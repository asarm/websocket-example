#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>

#define SERVER_PORT 8080

typedef struct SensorData {
  char roomName[5];
  float co2Concentration;
  float humidity;
  float light;
  float pir;
  float temperature;
} SensorData;

int storeCFilenames(char *filenames[]) {
  DIR *dir;
  struct dirent *ent;
  int i = 0;

  dir = opendir("data");
  if (dir == NULL) {
    return 0;
  }

  while ((ent = readdir(dir)) != NULL) {
    if (ent->d_name[0] == 'C') {
      filenames[i] = malloc(sizeof(ent->d_name));
      strcpy(filenames[i], ent->d_name);

      i++;
    }
  }

  closedir(dir);

  return i;
}

void removeFirstChar(char *str) {
    if (str == NULL || str[0] == '\0') {
        // Check for invalid input
        return;
    }

    // Shift characters to the left
    int i;
    for (i = 0; str[i] != '\0'; ++i) {
        str[i] = str[i + 1];
    }
    // Set the last character to null terminator
    str[i - 1] = '\0';
}

char** readCSV(const char *filepath, char *room_name, int time_index){
    char sensor_titles[5][15] = {"co2", "humidity", "light", "pir", "temperature"};
    char** room_data = malloc(5 * sizeof(char*));
    for (int i = 0; i < 5; i++) {
        room_data[i] = malloc(256);
        strcpy(room_data[i], "-1");
    }

    char row[100];

    char filename[100];
    char* var = "";

    for(int key_id=0;key_id<5;key_id++){
        int current_time = 0;

        snprintf(filename, sizeof(filename), "%s%s/%s.csv", filepath, room_name, sensor_titles[key_id]);
        FILE *file = fopen(filename, "r");

        if (file == NULL){
            printf("error while openning %s\n", filename);
        }

        while (fgets(row, 100, file) != NULL)
        {
            if(current_time == time_index){
                var = strchr(row, ' ');
                removeFirstChar(var); // remove the whitespace

                strcpy(room_data[key_id], var);

                break;

            }else{
                current_time++;
            }
        }
        fclose(file);
    }

    return room_data;
    }


void concatRoomData(SensorData* sensorData, char* globalString){
    char concatenatedString[40];
    snprintf(concatenatedString, sizeof(concatenatedString), "%s,%.2f,%.2f,%.2f,%.2f,%.2f",
             sensorData->roomName, sensorData->co2Concentration, sensorData->humidity,
             sensorData->light, sensorData->pir, sensorData->temperature);

    if (strlen(globalString) > 0) {
        strcat(globalString, "||");
    }
    strcat(globalString, concatenatedString);
}

int main() {
  char *data_path = "data/";
  int tempFileCount = 100;
  char *filenames[tempFileCount];
  int fileCount = storeCFilenames(filenames);

  printf("Room Count: %d\n", fileCount);

  // Initialize Winsock
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed: %d\n", iResult);
    return 1;
  }

  SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == INVALID_SOCKET) {
    printf("socket failed: %d\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET; // ipv4 address family
  serverAddr.sin_port = htons(SERVER_PORT);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost

   // Connect to the server
  iResult = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  if (iResult != 0) {
    printf("connect failed: %d\n", WSAGetLastError());
    closesocket(sockfd);
    WSACleanup();
    return 1;
  }

  SensorData sensorData;
  int frequency = 1; // Default frequency

  int timeIndex = 0;
  while (1) {
    char globalString[40*30+30] = "";

    printf("\nSystem Time: %d", timeIndex);

    for(int i=0; i<fileCount;i++){
        // mainpath, roomname, timeindex
        char** room_data = readCSV(data_path, filenames[i], timeIndex);
        
        snprintf(sensorData.roomName, sizeof(sensorData.roomName), "%s", filenames[i]);

        sensorData.co2Concentration = strtof(room_data[0], NULL);
        sensorData.humidity = strtof(room_data[1], NULL);
        sensorData.light = strtof(room_data[2], NULL);
        sensorData.pir = strtof(room_data[3], NULL);
        sensorData.temperature = strtof(room_data[4], NULL);

        concatRoomData(&sensorData, globalString);
    }

    iResult = send(sockfd, &globalString, sizeof(globalString), 0);

    if (iResult == SOCKET_ERROR) {
      printf("send failed: %d\n", WSAGetLastError());
      break;
    }

    sleep(0.01);
    timeIndex++;
  }

  closesocket(sockfd);
  WSACleanup();

  return 0;
}
