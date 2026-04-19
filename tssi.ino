/* * Tactical SSI - Multi-Intent Detection
 * Logic: Single Swallow = "SWALLOW", Double Swallow (within 2s) = "DON'T MOVE"
 */

const int bioAmpPin = D0;
float filteredEnergy = 0;
const float smoothing = 0.05; 

// --- PARAMETERS ---
const int NOISE_DEADZONE = 8000;  
const int TRIGGER_LIMIT = 15000;  
const unsigned long DOUBLE_TAP_WINDOW = 2000; // 2 seconds window

// --- STATE TRACKING ---
unsigned long lastTriggerTime = 0;
int swallowCount = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Serial.println(">>> SYSTEM ARMED: MONITORING MULTI-INTENT <<<");
}

void loop() {
  long burstSum = 0;
  int rawCenter = 2048; 

  for (int i = 0; i < 60; i++) {
    int val = analogRead(bioAmpPin);
    burstSum += abs(val - rawCenter); 
    delayMicroseconds(300);
  }

  float currentEnergy = (burstSum / 10.0) * 15.0; 
  filteredEnergy = (smoothing * currentEnergy) + ((1.0 - smoothing) * filteredEnergy);
  float netSignal = currentEnergy - filteredEnergy;
  float tacticalSignal = (netSignal < NOISE_DEADZONE) ? 0 : netSignal;

  // Plotting Data
  Serial.print("Baseline:");
  Serial.print(filteredEnergy);
  Serial.print(",");
  Serial.print("Tactical_Signal:");
  Serial.print(tacticalSignal);
  Serial.print(",");
  Serial.print("Limit:");
  Serial.println(TRIGGER_LIMIT);

  // --- DETECTION LOGIC ---
  if (tacticalSignal > TRIGGER_LIMIT) {
    unsigned long currentTime = millis();
    
    // Check if this spike happened within 2 seconds of the last one
    if (currentTime - lastTriggerTime < DOUBLE_TAP_WINDOW) {
      Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      Serial.println(">>> ALERT: DON'T MOVE <<<");
      Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      lastTriggerTime = 0; // Reset to avoid triple-triggering
    } else {
      Serial.println("***************************");
      Serial.println(">>> CAREFUL: MOVE <<<");
      Serial.println("***************************");
      lastTriggerTime = currentTime;
    }
    
    delay(800); // Shorter delay to allow for a quick second swallow
  }
}
