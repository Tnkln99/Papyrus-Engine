void setup() 
{
  Serial.begin(9600);
  pinMode(12, OUTPUT);
}

void loop() 
{
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Read until newline
    if (input == "<cmd:LED_ON>") {
      digitalWrite(12, HIGH);
      Serial.println("<resp:LED_ON_ACK>");
    } else if (input == "<cmd:LED_OFF>") {
      digitalWrite(12, LOW);
      Serial.println("<resp:LED_OFF_ACK>");
    }
  }
}
