#pragma once

void handleReceiver();
void receiverLoop();
void handleRecepteurRefresh();
void handleRecepteurDetails();
void handleScan();
#if defined(ESP8266)
void handleNothing();
#endif
//static void wifi_sniffer_cb(void *recv_buf, wifi_promiscuous_pkt_type_t type);
