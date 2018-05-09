#include <Adafruit_NeoPixel.h>
#include "config.h"

// edit these in config.h
extern const int pin;
extern const int n_pixels; 

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(n_pixels, pin, NEO_GRB + NEO_KHZ800);

int k[n_pixels];
int s[n_pixels];

float globalBrightness = 0;
int overallBrightness = 100;

bool lampStatus = false;
bool targetStatus = false;
bool changeStatus = false;

struct RGB{
  double r;
  double g;
  double b;  
};

struct HSV {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
};

int lamp[n_pixels];

RGB ist[n_pixels/2];
RGB soll[n_pixels/2];

RGB colors[n_pixels];

void setup() {

  setupNetworking();

  pixels.begin();

  for(int i = 0; i < n_pixels/2; i++){
    lamp[i] = i;    
    lamp[i+(n_pixels/2)] = (n_pixels-1) - i;
  }

  for(int i = 0; i < n_pixels; i++){
    k[i] = random(6280);
    s[i] = random(20, 40);
  }

}


void loop() {

  loopNetworking();

  if(targetStatus != lampStatus){
    if(targetStatus == 1){
      if(globalBrightness < 0.99f){
        globalBrightness += 0.01;
       } else {
        globalBrightness = 1;
        lampStatus = 1;
        updateStatus(true);
      }
    }
    if(targetStatus == 0){
      if(globalBrightness > 0.01f){
        globalBrightness -= 0.01;
       } else {
        globalBrightness = 0;
        lampStatus = 0;      
        updateStatus(false);
      }
    }
  }
  
  drops();
  drawpixels();

}

void drops(){

  int trigger = random(200);
  if(trigger == 1){
    int drop = random(74);

    float h = (float)random(360);
    float s = 1.0f;
    float v = 1.0f; //0.7f + (float)random(30)/100;

    HSV dropHSV = {h,s,v};
    RGB dropColor = hsv2rgb(dropHSV);
      
    for(int i = 0; i < n_pixels/2; i++){
      double l = 20;
      double brightness = abs(drop - i) > l ? 0 : ( l - abs(drop - i) ) / l ;
      if(brightness > 0){
        soll[i].r = (ist[i].r + dropColor.r * brightness)/2;
        soll[i].g = (ist[i].g + dropColor.g * brightness)/2;
        soll[i].b = (ist[i].b + dropColor.b * brightness)/2;
      }

    }


  }
  
  for(int i = 0; i < n_pixels/2; i++){
    ist[i].r -= (ist[i].r - soll[i].r)/200;
    ist[i].g -= (ist[i].g - soll[i].g)/200;
    ist[i].b -= (ist[i].b - soll[i].b)/200;

    soll[i].r *= 0.9995; 
    soll[i].g *= 0.9995;
    soll[i].b *= 0.9995;
    
    setcolorCol(i, ist[i].r, ist[i].g, ist[i].b);
  }
}

void drawpixels(){
  for(int i=0;i<n_pixels;i++){

    k[i] += s[i];
    float brightness = (sin((float)k[i]/1000) + 1) / 2;

    int r = ((colors[i].r * 255.0f) * brightness) * globalBrightness * ((float)overallBrightness / 100.0f);
    int g = ((colors[i].g * 255.0f) * brightness) * globalBrightness * ((float)overallBrightness / 100.0f);
    int b = ((colors[i].b * 255.0f) * brightness) * globalBrightness * ((float)overallBrightness / 100.0f);

    pixels.setPixelColor(lamp[i], pixels.Color( r, g, b ) );
    if(k[i] > (PI*2)*1000) k[i] = 0;
  }
  pixels.show(); 
}

void setcolorCol(int i, double r, double g, double b){

  colors[i] = {r,g,b};
  colors[i+(n_pixels/2)] = {r,g,b};
  
}

RGB hsv2rgb(HSV hsv)
{
    RGB rgb;
    double H = hsv.h, S = hsv.s, V = hsv.v,
            P, Q, T,
            fract;

    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);

    P = V*(1. - S);
    Q = V*(1. - S*fract);
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
        rgb = (RGB){.r = V, .g = T, .b = P};
    else if (1. <= H && H < 2.)
        rgb = (RGB){.r = Q, .g = V, .b = P};
    else if (2. <= H && H < 3.)
        rgb = (RGB){.r = P, .g = V, .b = T};
    else if (3. <= H && H < 4.)
        rgb = (RGB){.r = P, .g = Q, .b = V};
    else if (4. <= H && H < 5.)
        rgb = (RGB){.r = T, .g = P, .b = V};
    else if (5. <= H && H < 6.)
        rgb = (RGB){.r = V, .g = P, .b = Q};
    else
        rgb = (RGB){.r = 0., .g = 0., .b = 0.};

    return rgb;
}

