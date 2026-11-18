#pragma once
#include "Arduino.h"
#define ZB_RX_RESPONSE 0x90
struct XBeeAddress64 { XBeeAddress64() {} XBeeAddress64(uint32_t, uint32_t) {} };
struct ZBRxResponse {
  std::vector<uint8_t> data;
  uint8_t* getData() { return data.data(); }
  uint8_t getDataLength() { return (uint8_t)data.size(); }
};
struct ZBTxRequest {
  std::vector<uint8_t> payload;
  ZBTxRequest() {}
  ZBTxRequest(XBeeAddress64, uint8_t* p, uint8_t n) : payload(p, p + n) {}
};
struct XBeeResponse {
  bool avail = false;
  bool isAvailable() { return avail; }
  uint8_t getApiId() { return ZB_RX_RESPONSE; }
  void getZBRxResponse(ZBRxResponse&) {}
};
struct XBee {
  XBeeResponse resp;
  std::vector<ZBTxRequest> sent;
  void begin(MockSerial&) {}
  void readPacket() {}
  XBeeResponse& getResponse() { return resp; }
  void send(ZBTxRequest& t) { sent.push_back(t); }
};
