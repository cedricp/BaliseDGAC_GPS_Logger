#pragma once

void handleReceiver();
void receiverLoop();
void handleRecepteurRefresh();
void handleRecepteurDetails();
void handleScan();
#if defined(ESP8266)
void handleNothing();
#endif
