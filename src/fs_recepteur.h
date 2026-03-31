#pragma once

void handleRecepteur();
void loopRecepteur();
void handleRecepteurRefresh();
void handleRecepteurDetail();
void handleScan();
static void initialize_wifi_sniffer();
#if defined(ESP8266)
void handleNothing();
#endif
//static void wifi_sniffer_cb(void *recv_buf, wifi_promiscuous_pkt_type_t type);
