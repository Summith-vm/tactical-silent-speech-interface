/* * Tactical SSI - Defense Grade Intent Detection
 * Target: Seeed Studio XIAO ESP32-S3
 */

const int bioAmpPin = D0;
float filteredEnergy = 0;
const float smoothing = 0.05; 

// --- TUNED FOR YOUR SPECIFIC SCREENSHOT DATA ---
const int NOISE_DEADZONE = 8000; // Ignore orange jitter (like the 2k-5k ones in your pic)
const int TRIGGER_LIMIT = 12000;  // Real swallows are 100k+, so 50k is 100% safe

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Serial.println("SYSTEM ARMED: WAITING FOR INTENT...");
}

void loop() {
  long burstSum = 0;
  int rawCenter = 2048; 

  for (int i = 0; i < 60; i++) {
    int val = analogRead(bioAmpPin);
    burstSum += abs(val - rawCenter); 
    delayMicroseconds(300);
  }

  // Calculate current power
  float currentEnergy = (burstSum / 10.0) * 15.0; 

  // Dynamic baseline tracking
  filteredEnergy = (smoothing * currentEnergy) + ((1.0 - smoothing) * filteredEnergy);

  // Calculate the Spike above the moving baseline
  float netSignal = currentEnergy - filteredEnergy;
  
  // Apply Deadzone: If it's just that idle jitter you saw, show 0
  float tacticalSignal = (netSignal < NOISE_DEADZONE) ? 0 : netSignal;

  // Output for Serial Plotter
  Serial.print("Baseline:");
  Serial.print(filteredEnergy);
  Serial.print(",");
  Serial.print("Tactical_Signal:");
  Serial.print(tacticalSignal);
  Serial.print(",");
  Serial.print("Limit:");
  Serial.println(TRIGGER_LIMIT);

  // Final Intent Logic
  if (tacticalSignal > TRIGGER_LIMIT) {
    Serial.println("-------------------------");
    Serial.println(">>> DETECTED: SWALLOW <<<");
    Serial.println("-------------------------");
    delay(1500); // Prevents double-triggers
  }
}
