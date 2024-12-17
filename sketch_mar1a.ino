#include <RTClib.h>
#include <NewPing.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#define dht_type 11
RTC_DS1307 rtc;
class basic_tasks{
  public:
  int pin_of_object;
  basic_tasks(int pin_number_of_object){
    pin_of_object = pin_number_of_object;
  }
  void on();
  void off();

};
void basic_tasks::on(){
  digitalWrite(pin_of_object , HIGH);
}
void basic_tasks::off(){
  digitalWrite(pin_of_object , LOW);
}



int time_switch(int hour_needed , int start_min , int end_min){
  
  DateTime now = rtc.now();
  int pin_state;
  int mins_rn = now.minute();
  int hours_rn = now.hour();
  if(hours_rn == hour_needed && mins_rn >= start_min && mins_rn <= end_min){
    pin_state = 1;
  }
  else{
    pin_state = 0;
  }
  return pin_state;


}




class get_water_volume{
  public:
  NewPing ultrasonic;
  get_water_volume(int triger_pin , int eco_pin) :ultrasonic(triger_pin , eco_pin){

  }
  int tank_hight;
  int side1;
  int side2;
  void begin(int hight , int length , int width);
  int get_water_hight(int iteration_num);
  int calculate_water_volume(int iteration_num);

};
void get_water_volume::begin(int hight, int length, int width){
  this->tank_hight = hight;
  this->side1 = length;
  this->side2 = width;
}
int get_water_volume::get_water_hight(int iteration_num){
  int duration = ultrasonic.ping_median(iteration_num);
  int air_hight = ultrasonic.convert_cm(duration);
  int water_hight  = tank_hight - air_hight;
  return water_hight;
}
int get_water_volume::calculate_water_volume(int iteration_num){
  int water_hight = get_water_hight(iteration_num);
  int water_volume = side1*side2*water_hight/1000;
  return water_volume;
}

class dht_smoothing {
  public:
  DHT ht;
  dht_smoothing(int pin, int type) : ht(pin, type) {
    ht.begin();
  }
  float tempC(int smooth_temp_time);
  float humid(int smooth_humid_time);
};
float dht_smoothing::tempC(int smooth_temp_time){
  float* temp_array = new float[smooth_temp_time];
  int delay_time = 1000 / smooth_temp_time;
  float sum_of_temps = 0;
  for(int i = 1 ; i <= smooth_temp_time ; i++ ){
    temp_array[i] = ht.readTemperature();
    delay(delay_time);
    sum_of_temps += temp_array[i];
  }
  float smoothed_temp = sum_of_temps /  smooth_temp_time;
  delete[] temp_array;
  return smoothed_temp;
}
float dht_smoothing::humid(int smooth_humid_time){
  float* humid_array = new float[smooth_humid_time];
  int delay_time = 1000 / smooth_humid_time;
  float sum_of_humids = 0;
  for(int i = 1 ; i <= smooth_humid_time ; i++ ){
    humid_array[i] = ht.readHumidity();
    delay(delay_time);
    sum_of_humids += humid_array[i];
  }
  float smoothed_humid = sum_of_humids /  smooth_humid_time;
  delete[] humid_array;
  return smoothed_humid;
}

  
dht_smoothing ht(6,dht_type);
get_water_volume water_sensor(3,4);
void setup() {
  rtc.begin();
  pinMode(13 , OUTPUT);
  Serial.begin(9600);   
  
}
void loop() {
  Serial.println(ht.tempC(3));
  Serial.print("                                   ");
  Serial.println(ht.humid(3));
}