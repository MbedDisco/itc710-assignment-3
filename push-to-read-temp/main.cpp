#include <mbed.h>
#include <http_request.h>
#include "HTS221Sensor.h"

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
Thread t;
EventQueue queue(5 * EVENTS_EVENT_SIZE);
Serial pc(USBTX, USBRX);
WiFiInterface *wifi;
static DevI2C devI2c(PB_11,PB_10);
static HTS221Sensor hum_temp(&devI2c);
uint8_t id;
float value1, value2;
//  char buffer1[32], buffer2[32];
int32_t axes[3];

const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_wifi() {
    WiFiAccessPoint *ap;

    printf("Scan:\n");
    int count = wifi->scan(NULL,0);
    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;
    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);
    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\n", count);

    delete[] ap;   

    return count; 
}
void connect_wifi() {

    pc.printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    int cb=1;
    while (cb){
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        pc.printf("\nConnection error: %d\n", ret);
    }
    else {
        pc.printf("\nSuccess!\n");
        cb=0; 
    }
   }
}





void pressed_handler() {
//    int count;

    connect_wifi();
//    if (count == 0) {
//        pc.printf("No WIFI APs found - can't continue further.\n");
//        return;
//    }

//    pc.printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
//    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
//    if (ret != 0) {
//        pc.printf("\nConnection error: %d\n", ret);
//        return;
//    }

//    pc.printf("Success\n\n");

//pc.printf("MAC: %s\n", wifi->get_mac_address());
//    pc.printf("IP: %s\n", wifi->get_ip_address());
//    pc.printf("Netmask: %s\n", wifi->get_netmask());
//    pc.printf("Gateway: %s\n", wifi->get_gateway());
//    pc.printf("RSSI: %d\n\n", wifi->get_rssi());
//NetworkInterface* network = /* obtain a NetworkInterface object */
 
 
 
    while(1) {
    hum_temp.get_temperature(&value1);
    hum_temp.get_humidity(&value2);
    string mcu2httpst = "http://dry-island-70890.herokuapp.com/update?temp="+to_string(value1)+"&hum="+to_string(value2);
    const char * mcu2http = mcu2httpst.c_str();
    pc.printf(mcu2http);
    pc.printf("\n");
    HttpRequest* request = new HttpRequest(wifi, HTTP_GET, mcu2http);
    request->set_header("Content-Type", "application/json");
    HttpResponse* response = request->send();
    // if response is NULL, check response->get_error()
    if (response == NULL) {
          pc.printf("network may disconnect, reconnecting\n");
          connect_wifi();
    }else {
    pc.printf("status is %d - %s\n", response->get_status_code(), response->get_status_message());
    pc.printf("body is:\n%s\n", response->get_body_as_string().c_str());
    }
    delete request; // also clears out the response
    pc.printf("HTS221:  [temp] %.2f C, [hum]   %.2f%%\r\n", value1, value2);
        ThisThread::sleep_for(10000);
    }

}

int main() {
    hum_temp.init(NULL);
    hum_temp.enable();
    hum_temp.read_id(&id);
    pc.printf("HTS221  humidity & temperature    = 0x%X\r\n", id);
    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    t.start(callback(&queue, &EventQueue::dispatch_forever));
    button.fall(queue.event(pressed_handler));
    pc.printf("Starting\n");
    while(1) {
        led = !led;
        ThisThread::sleep_for(500);
    }
}
