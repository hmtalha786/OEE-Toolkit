#include <SD.h>
File data_file;

int file_count = 0;
int pack_count = 0;
int temp = 0;
int file_array = 0;
char *myStrings[] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt", "file5.txt", "file6.txt" };
String fileToWrite = myStrings[file_array];

void writeToSD(int values) {
  data_file = SD.open(fileToWrite, FILE_WRITE);
  delay(100);
  if (data_file) {
    data_file.print(values);
    data_file.print(",");
    delay(100);
    data_file.close();
    delay(100);
  }
  if (pack_count > 30) {
    pack_count = 0;
    if (file_array < 5) {
      file_array++;
    }
    else if (file_array >= 5) {
      file_array = 0;
    }
    SD.remove(myStrings[file_array]);
    Serial.print("Deleting ");
    Serial.println(myStrings[file_array]);
    fileToWrite = myStrings[file_array];
    delay(100);
  }
}

void setup() {
  Serial.begin(9600);
  if (SD.begin(53)) {
    Serial.println("SD Card Connected");
    digitalWrite(Q0_0, HIGH);
    digitalWrite(Q0_1, HIGH);
    delay(100);
    for (int i = 0; i < 6; i++) {
      Serial.print("deleting ");
      Serial.println(myStrings[i]);
      SD.remove(myStrings[i]);
      delay(500);
    }
    delay(100);
  } else {
    Serial.println("SD Card not Connected");
    digitalWrite(Q0_0, LOW);
    digitalWrite(Q0_1, LOW);
  }
}

void loop() {
  temp++;
  pack_count++;
  writeToSD(temp);
  Serial.print("Current file: ");
  Serial.println(fileToWrite);
  delay(1000);
}
