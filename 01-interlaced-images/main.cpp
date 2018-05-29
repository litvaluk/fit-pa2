#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cctype>
#include <climits>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
using namespace std;

const uint16_t ENDIAN_LITTLE = 0x4949;
const uint16_t ENDIAN_BIG    = 0x4d4d;

#endif /* __PROGTEST__ */

uint16_t to2B(char a, char b){
  int x = (unsigned int)((unsigned char)a);
  int y = (unsigned int)((unsigned char)b);

  return (y * 256) + x;
}

void from2B(uint16_t from, char& a, char& b){
  b = from/256;
  a = from%256;
}

string toBinary(int n){
    string tmp;
    while(n != 0){
      tmp = (n % 2 == 0 ? "0" : "1") + tmp;
      n /= 2;
    }
    string str = "0000000000000000";
    int j = 15;
    for (int i = tmp.length() - 1; i >= 0; i--) {
      str[j] = tmp[i];
      j--;
    }
    return str;
}

int fromBinary(string str){
  int res = 0;
  int j = 0;
  for (int i = str.length() - 1; i >= 0; i--) {
    if (str[i] == 49) {
      res += pow(2, j);
    }
    j++;
  }
  return res;
}

class CImage{

private:
  int m_width, m_height, m_channels, m_channelBits, m_interleave, m_endian;
  string m_bitsetStr;
  int* pixels;
  int header[8];
  bool isValid;

  int interleaveFromFormat(char a, char b, char c){

    if(a == '0' && b == '0' && c == '0'){
      return 1;
    }
    else if(a == '0' && b == '0' && c == '1'){
      return 2;
    }
    else if(a == '0' && b == '1' && c == '0'){
      return 4;
    }
    else if(a == '0' && b == '1' && c == '1'){
      return 8;
    }
    else if(a == '1' && b == '0' && c == '0'){
      return 16;
    }
    else if(a == '1' && b == '0' && c == '1'){
      return 32;
    }
    else if(a == '1' && b == '1' && c == '0'){
      return 64;
    }
    else{
      //cout << "Spatna kombinace" << endl;
      return -1;
    }

  }

  int channelBitsFromFormat(char a, char b, char c){

    if(a == '0' && b == '0' && c == '0'){
      return 1;
    }
    else if(a == '0' && b == '1' && c == '1'){
      return 8;
    }
    else if(a == '1' && b == '0' && c == '0'){
      return 16;
    }
    else{
      //cout << "Spatna kombinace" << endl;
      return -1;
    }

  }

  int channelsFromFormat(char a, char b){

    if(a == '0' && b == '0'){
      return 1;
    }
    else if(a == '1' && b == '0'){
      return 3;
    }
    else if(a == '1' && b == '1'){
      return 4;
    }
    else{
      //cout << "Spatna kombinace" << endl;
      return -1;
    }

  }

  bool decodeFormat(string f){
    for (int i = 0; i < 8; i++) {
      if (f[i] != '0') {
        //cout << "nenula v nulovem prostoru" << endl;
        return false;
      }
    }

    m_interleave = interleaveFromFormat(f[8], f[9], f[10]);
    m_channelBits = channelBitsFromFormat(f[11], f[12], f[13]);
    m_channels = channelsFromFormat(f[14], f[15]);

    if(m_interleave != 1 && m_interleave != 2
      && m_interleave != 4 && m_interleave != 8
      && m_interleave != 16 && m_interleave != 32
      && m_interleave != 64){
        //cout << "spatny format prokladani v obrazku" << endl;
        return false;
    }
    if (m_channelBits != 1 && m_channelBits != 8 && m_channelBits != 16) {
      //cout << "spatny format poctu bitu na kanal" << endl;
      return false;
    }
    if (m_channels != 1 && m_channels != 3 && m_channels != 4 ) {
      //cout << "spatny pocet kanalu" << endl;
      return false;
    }

    return true;
  }

public:
  CImage(char*& f, int size){

    for (int i = 0; i < 8; i++) {
      header[i] = f[i];
    }

    m_endian = to2B(f[0], f[1]);

    if(m_endian == 0x4d4d){
      for (int i = 2; i < size; i+=2) {
        char tmp = f[i];
        f[i] = f[i+1];
        f[i+1] = tmp;
      }
    }

    m_width = to2B(f[2], f[3]);
    m_height = to2B(f[4], f[5]);

    for (int i = 0; i < 8; i++) {
      header[i] = f[i];
    }

    if (m_endian != 0x4949 && m_endian != 0x4d4d) {
      //cout << "spatny endian v obrazku" << endl;
      isValid = false;
      return;
    }

    if (m_width == 0 || m_height == 0) {
      //cout << "jeden nebo oba rozmery jsou nulove" << endl;
      isValid = false;
      return;
    }

    m_bitsetStr = toBinary(to2B(f[6], f[7]));

    isValid = decodeFormat(m_bitsetStr);

    // set valid to false when size is not headersize+width+height+channels
    if(m_channelBits == 8 && size != 8 + m_width*m_height*m_channels){
      //cout << "sirka a vyska nekoresponduje s poctem bitu (8b)" << endl;
      isValid = false;
    }
    if(m_channelBits == 16 && size != 8 + 2*m_width*m_height*m_channels){
      //cout << "sirka a vyska nekoresponduje s poctem bitu (16b)" << endl;
      isValid = false;
    }

    // validity check
    if (!isValid) {
      return;
    }

    pixels = new int[m_width*m_height*m_channels];

    if (m_channels == 1) {
      if (m_channelBits == 8) {
        for (int i = 0; i < m_width*m_height*m_channels; i++) {
          pixels[i] = (unsigned int)((unsigned char)f[i+8]);
        }
      }
      if (m_channelBits == 16) {
        int j = 8;
        for (int i = 0; i < m_width*m_height*m_channels; i++) {
          pixels[i] = to2B((unsigned int)((unsigned char)f[j]), (unsigned int)((unsigned char)f[j+1]));
          j += 2;
        }
      }
      /*cout << "Pixel " << i+1 << " = " << pixels[i] << endl;*/
    }

    else if (m_channels == 3) {
      if (m_channelBits == 8) {
        for (int i = 0; i < m_width*m_height*m_channels; i+=3) {
          pixels[i] = (unsigned int)((unsigned char)f[i+8]);
          pixels[i+1] = (unsigned int)((unsigned char)f[i+8+1]);
          pixels[i+2] = (unsigned int)((unsigned char)f[i+8+2]);
        }
      }
      if (m_channelBits == 16) {
        int j = 8;
        for (int i = 0; i < m_width*m_height*m_channels; i+=3) {
          pixels[i] = to2B((unsigned int)((unsigned char)f[j]), (unsigned int)((unsigned char)f[j+1]));
          pixels[i+1] = to2B((unsigned int)((unsigned char)f[j+2]), (unsigned int)((unsigned char)f[j+3]));
          pixels[i+2] = to2B((unsigned int)((unsigned char)f[j+4]), (unsigned int)((unsigned char)f[j+5]));
          j += 3*2;
        }
      }
      /*cout << "Pixel " << i/3+1 << ": r=" << pixels[i] <<
        " g=" << pixels[i+1] << " b=" << pixels[i+2] <<  endl;*/
    }

    else if (m_channels == 4) {
      if (m_channelBits == 8) {
        for (int i = 0; i < m_width*m_height*m_channels; i+=4) {
          pixels[i] = (unsigned int)((unsigned char)f[i+8]);
          pixels[i+1] = (unsigned int)((unsigned char)f[i+8+1]);
          pixels[i+2] = (unsigned int)((unsigned char)f[i+8+2]);
          pixels[i+3] = (unsigned int)((unsigned char)f[i+8+3]);
        }
      }
      if (m_channelBits == 16) {
        int j = 8;
        for (int i = 0; i < m_width*m_height*m_channels; i+=4) {
          pixels[i] = to2B((unsigned int)((unsigned char)f[j]), (unsigned int)((unsigned char)f[j+1]));
          pixels[i+1] = to2B((unsigned int)((unsigned char)f[j+2]), (unsigned int)((unsigned char)f[j+3]));
          pixels[i+2] = to2B((unsigned int)((unsigned char)f[j+4]), (unsigned int)((unsigned char)f[j+5]));
          pixels[i+3] = to2B((unsigned int)((unsigned char)f[j+6]), (unsigned int)((unsigned char)f[j+7]));
          j += 4*2;
        }
      }
      /*cout << "Pixel " << i/4+1 << ": r=" << pixels[i] <<
        " g=" << pixels[i+1] << " b=" << pixels[i+2] <<
        " a=" << pixels[i+3] << endl;*/
    }
    isValid = true;
  }
  ~CImage(){
    if (isValid) {
      delete[] pixels;
    }
  }
  int getWidth(){
    return m_width;
  }
  int getHeight(){
    return m_height;
  }
  int getChannels(){
    return m_channels;
  }
  int getChannelBits(){
    return m_channelBits;
  }
  int getInterleave(){
    return m_interleave;
  }
  bool getIsValid(){
    return isValid;
  }

  void printInfo(){
    cout << hex << "Endian: 0x" << m_endian << dec << endl;
    cout << "Sirka: " << m_width << endl;
    cout << "Vyska: " << m_height << endl;
    cout << "Pocet kanalu: " << m_channels << endl;
    cout << "Bitu na kanal: " << m_channelBits << endl;
    cout << "Prokladani: " << m_interleave << endl;
  }

  void printPixels(){
    switch (m_channels) {
      case 1:
        for (int i = 0; i < m_width*m_height*m_channels; i++) {
          cout << "Pixel " << i+1 << " = " << pixels[i] << endl;
        }
        break;
      case 3:
        for (int i = 0; i < m_width*m_height*m_channels; i+=3) {
          cout << "Pixel " << i/3+1 << ": r=" << pixels[i] <<
            " g=" << pixels[i+1] << " b=" << pixels[i+2] <<  endl;
        }
        break;
      case 4:
        for (int i = 0; i < m_width*m_height*m_channels; i+=4) {
          cout << "Pixel " << i/4+1 << ": r=" << pixels[i] <<
            " g=" << pixels[i+1] << " b=" << pixels[i+2] <<
            " a=" << pixels[i+3] << endl;
        }
        break;
    }
  }

  char* toString(uint16_t toEndian){
    int size = 0;

    if (m_channelBits == 8) {
      size = 8 + m_width*m_height*m_channels;
    }
    else if (m_channelBits == 16) {
      size = 8 + 2*m_width*m_height*m_channels;
    }
    char* tmp = new char[size];

    tmp[0] = header[0];
    tmp[1] = header[1];
    tmp[2] = header[2];
    tmp[3] = header[3];
    tmp[4] = header[4];
    tmp[5] = header[5];
    tmp[6] = fromBinary(m_bitsetStr);
    tmp[7] = 0;

    if (m_channelBits == 8) {
      for (int i = 8; i < size; i++) {
        tmp[i] = pixels[i-8];
      }
    }
    else if (m_channelBits == 16) {
      for (int i = 0; i < m_width*m_height*m_channels; i++) {
        char a;
        char b;
        from2B(pixels[i], a, b);
        tmp[2*i+8] = a;
        tmp[2*i+8+1] = b;
      }
    }

    convertEndian(toEndian, tmp, size);
    return tmp;
  }

  void convertEndian(uint16_t toEndian, char*& f, int size){
    if (m_endian == ENDIAN_LITTLE && toEndian == ENDIAN_BIG) {
      f[0] = 'M';
      f[1] = 'M';
      for (int i = 2; i < size; i+=2) {
        char tmp = f[i];
        f[i] = f[i+1];
        f[i+1] = tmp;
      }
    }
    else if (m_endian == ENDIAN_BIG && toEndian == ENDIAN_LITTLE) {
      f[0] = 'I';
      f[1] = 'I';
    }
    else if (m_endian == ENDIAN_BIG && toEndian == ENDIAN_BIG) {
      for (int i = 2; i < size; i+=2) {
        char tmp = f[i];
        f[i] = f[i+1];
        f[i+1] = tmp;
      }
    }
  }

  void interleave(int factor){

      if(factor == 1){
        return;
      }

      int* tmp = new int[m_width*m_height*m_channels];
      for (int i = 0; i < m_width*m_height*m_channels; i++) {
        tmp[i] = pixels[i];
      }

      int cnt = 0;

      if (factor > 32) {
        for(int h = 0; h < m_height; h += 64){
          for (int w = 0; w < m_width*m_channels; w += 64*m_channels) {
            if (pixels[h*m_width*m_channels+w] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w];
              pixels[h*m_width*m_channels+w] = -64;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+1] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+1];
              pixels[h*m_width*m_channels+w+1] = -64;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+2] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+2];
              pixels[h*m_width*m_channels+w+2] = -64;
              cnt++;
            }
            if (m_channels > 3 && pixels[h*m_width*m_channels+w+3] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+3];
              pixels[h*m_width*m_channels+w+3] = -64;
              cnt++;
            }
          }
        }
      }

      if (factor > 16) {
        for(int h = 0; h < m_height; h += 32){
          for (int w = 0; w < m_width*m_channels; w += 32*m_channels) {
            if (pixels[h*m_width*m_channels+w] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w];
              pixels[h*m_width*m_channels+w] = -32;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+1] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+1];
              pixels[h*m_width*m_channels+w+1] = -32;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+2] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+2];
              pixels[h*m_width*m_channels+w+2] = -32;
              cnt++;
            }
            if (m_channels > 3 && pixels[h*m_width*m_channels+w+3] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+3];
              pixels[h*m_width*m_channels+w+3] = -32;
              cnt++;
            }
          }
        }
      }

      if (factor > 8) {
        for(int h = 0; h < m_height; h += 16){
          for (int w = 0; w < m_width*m_channels; w += 16*m_channels) {
            if (pixels[h*m_width*m_channels+w] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w];
              pixels[h*m_width*m_channels+w] = -16;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+1] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+1];
              pixels[h*m_width*m_channels+w+1] = -16;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+2] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+2];
              pixels[h*m_width*m_channels+w+2] = -16;
              cnt++;
            }
            if (m_channels > 3 && pixels[h*m_width*m_channels+w+3] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+3];
              pixels[h*m_width*m_channels+w+3] = -16;
              cnt++;
            }
          }
        }
      }

      if (factor > 4) {
        for(int h = 0; h < m_height; h += 8){
          for (int w = 0; w < m_width*m_channels; w += 8*m_channels) {
            if (pixels[h*m_width*m_channels+w] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w];
              pixels[h*m_width*m_channels+w] = -8;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+1] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+1];
              pixels[h*m_width*m_channels+w+1] = -8;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+2] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+2];
              pixels[h*m_width*m_channels+w+2] = -8;
              cnt++;
            }
            if (m_channels > 3 && pixels[h*m_width*m_channels+w+3] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+3];
              pixels[h*m_width*m_channels+w+3] = -8;
              cnt++;
            }
          }
        }
      }

      if(factor > 2){
        for(int h = 0; h < m_height; h += 4){
          for (int w = 0; w < m_width*m_channels; w += 4*m_channels) {
            if (pixels[h*m_width*m_channels+w] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w];
              pixels[h*m_width*m_channels+w] = -4;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+1] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+1];
              pixels[h*m_width*m_channels+w+1] = -4;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+2] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+2];
              pixels[h*m_width*m_channels+w+2] = -4;
              cnt++;
            }
            if (m_channels > 3 && pixels[h*m_width*m_channels+w+3] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+3];
              pixels[h*m_width*m_channels+w+3] = -4;
              cnt++;
            }
          }
        }
      }

      if (factor > 1) {
        for(int h = 0; h < m_height; h += 2){
          for (int w = 0; w < m_width*m_channels; w += 2*m_channels) {
            if (pixels[h*m_width*m_channels+w] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w];
              pixels[h*m_width*m_channels+w] = -2;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+1] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+1];
              pixels[h*m_width*m_channels+w+1] = -2;
              cnt++;
            }
            if (m_channels > 1 && pixels[h*m_width*m_channels+w+2] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+2];
              pixels[h*m_width*m_channels+w+2] = -2;
              cnt++;
            }
            if (m_channels > 3 && pixels[h*m_width*m_channels+w+3] >= 0) {
              tmp[cnt] = pixels[h*m_width*m_channels+w+3];
              pixels[h*m_width*m_channels+w+3] = -2;
              cnt++;
            }
          }
        }
      }

      for(int h = 0; h < m_height; h ++){
        for (int w = 0; w < m_width*m_channels; w+=1*m_channels) {
          if (pixels[h*m_width*m_channels+w] >= 0) {
            tmp[cnt] = pixels[h*m_width*m_channels+w];
            pixels[h*m_width*m_channels+w] = -1;
            cnt++;
          }
          if (m_channels > 1 && pixels[h*m_width*m_channels+w+1] >= 0) {
            tmp[cnt] = pixels[h*m_width*m_channels+w+1];
            pixels[h*m_width*m_channels+w+1] = -1;
            cnt++;
          }
          if (m_channels > 1 && pixels[h*m_width*m_channels+w+2] >= 0) {
            tmp[cnt] = pixels[h*m_width*m_channels+w+2];
            pixels[h*m_width*m_channels+w+2] = -1;
            cnt++;
          }
          if (m_channels > 3 && pixels[h*m_width*m_channels+w+3] >= 0) {
            tmp[cnt] = pixels[h*m_width*m_channels+w+3];
            pixels[h*m_width*m_channels+w+3] = -1;
            cnt++;
          }
        }
      }

      for (int i = 0; i < m_width*m_height*m_channels; i++) {
        pixels[i] = tmp[i];
      }

      if(factor == 1){
        m_bitsetStr[8] = '0';
        m_bitsetStr[9] = '0';
        m_bitsetStr[10] = '0';
      }
      if(factor == 2){
        m_bitsetStr[8] = '0';
        m_bitsetStr[9] = '0';
        m_bitsetStr[10] = '1';
      }
      if(factor == 4){
        m_bitsetStr[8] = '0';
        m_bitsetStr[9] = '1';
        m_bitsetStr[10] = '0';
      }
      if(factor == 8){
        m_bitsetStr[8] = '0';
        m_bitsetStr[9] = '1';
        m_bitsetStr[10] = '1';
      }
      if(factor == 16){
        m_bitsetStr[8] = '1';
        m_bitsetStr[9] = '0';
        m_bitsetStr[10] = '0';
      }
      if(factor == 32){
        m_bitsetStr[8] = '1';
        m_bitsetStr[9] = '0';
        m_bitsetStr[10] = '1';
      }
      if(factor == 64){
        m_bitsetStr[8] = '1';
        m_bitsetStr[9] = '1';
        m_bitsetStr[10] = '0';
      }

      m_interleave = factor;

      delete[] tmp;
  }

  void revert(){

    if (m_interleave == 1) {
      return;
    }

    int* tmp = new int[m_width*m_height*m_channels];
    for (int i = 0; i < m_width*m_height*m_channels; i++) {
      tmp[i] = -1;
    }

    //int cnt = 0;
    int factor = m_interleave;
    for (int i = 0; i < m_width*m_height*m_channels; ) {

      if (factor > 32) {
        for(int h = 0; h < m_height; h += 64){
          for (int w = 0; w < m_width*m_channels; w += 64*m_channels) {
            if (tmp[h*m_width*m_channels+w] < 0) {
              tmp[h*m_width*m_channels+w] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+1] < 0) {
              tmp[h*m_width*m_channels+w+1] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+2] < 0) {
              tmp[h*m_width*m_channels+w+2] = pixels[i];
              i++;
            }
            if (m_channels > 3 && tmp[h*m_width*m_channels+w+3] < 0) {
              tmp[h*m_width*m_channels+w+3] = pixels[i];
              i++;
            }
          }
        }
      }

      if (factor > 16) {
        for(int h = 0; h < m_height; h += 32){
          for (int w = 0; w < m_width*m_channels; w += 32*m_channels) {
            if (tmp[h*m_width*m_channels+w] < 0) {
              tmp[h*m_width*m_channels+w] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+1] < 0) {
              tmp[h*m_width*m_channels+w+1] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+2] < 0) {
              tmp[h*m_width*m_channels+w+2] = pixels[i];
              i++;
            }
            if (m_channels > 3 && tmp[h*m_width*m_channels+w+3] < 0) {
              tmp[h*m_width*m_channels+w+3] = pixels[i];
              i++;
            }
          }
        }
      }

      if (factor > 8) {
        for(int h = 0; h < m_height; h += 16){
          for (int w = 0; w < m_width*m_channels; w += 16*m_channels) {
            if (tmp[h*m_width*m_channels+w] < 0) {
              tmp[h*m_width*m_channels+w] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+1] < 0) {
              tmp[h*m_width*m_channels+w+1] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+2] < 0) {
              tmp[h*m_width*m_channels+w+2] = pixels[i];
              i++;
            }
            if (m_channels > 3 && tmp[h*m_width*m_channels+w+3] < 0) {
              tmp[h*m_width*m_channels+w+3] = pixels[i];
              i++;
            }
          }
        }
      }

      if (factor > 4) {
        for(int h = 0; h < m_height; h += 8){
          for (int w = 0; w < m_width*m_channels; w += 8*m_channels) {
            if (tmp[h*m_width*m_channels+w] < 0) {
              tmp[h*m_width*m_channels+w] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+1] < 0) {
              tmp[h*m_width*m_channels+w+1] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+2] < 0) {
              tmp[h*m_width*m_channels+w+2] = pixels[i];
              i++;
            }
            if (m_channels > 3 && tmp[h*m_width*m_channels+w+3] < 0) {
              tmp[h*m_width*m_channels+w+3] = pixels[i];
              i++;
            }
          }
        }
      }

      if (factor > 2) {
        for(int h = 0; h < m_height; h += 4){
          for (int w = 0; w < m_width*m_channels; w += 4*m_channels) {
            if (tmp[h*m_width*m_channels+w] < 0) {
              tmp[h*m_width*m_channels+w] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+1] < 0) {
              tmp[h*m_width*m_channels+w+1] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+2] < 0) {
              tmp[h*m_width*m_channels+w+2] = pixels[i];
              i++;
            }
            if (m_channels > 3 && tmp[h*m_width*m_channels+w+3] < 0) {
              tmp[h*m_width*m_channels+w+3] = pixels[i];
              i++;
            }
          }
        }
      }


      if (factor > 1) {
        for(int h = 0; h < m_height; h += 2){
          for (int w = 0; w < m_width*m_channels; w += 2*m_channels) {
            if (tmp[h*m_width*m_channels+w] < 0) {
              tmp[h*m_width*m_channels+w] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+1] < 0) {
              tmp[h*m_width*m_channels+w+1] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+2] < 0) {
              tmp[h*m_width*m_channels+w+2] = pixels[i];
              i++;
            }
            if (m_channels > 3 && tmp[h*m_width*m_channels+w+3] < 0) {
              tmp[h*m_width*m_channels+w+3] = pixels[i];
              i++;
            }
          }
        }
      }

        for(int h = 0; h < m_height; h += 1){
          for (int w = 0; w < m_width*m_channels; w += 1*m_channels) {
            if (tmp[h*m_width*m_channels+w] < 0) {
              tmp[h*m_width*m_channels+w] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+1] < 0) {
              tmp[h*m_width*m_channels+w+1] = pixels[i];
              i++;
            }
            if (m_channels > 1 && tmp[h*m_width*m_channels+w+2] < 0) {
              tmp[h*m_width*m_channels+w+2] = pixels[i];
              i++;
            }
            if (m_channels > 3 && tmp[h*m_width*m_channels+w+3] < 0) {
              tmp[h*m_width*m_channels+w+3] = pixels[i];
              i++;
            }
          }
        }

    }

    for (int i = 0; i < m_width*m_height*m_channels; i++) {
      pixels[i] = tmp[i];
    }

    m_bitsetStr[8] = '0';
    m_bitsetStr[9] = '0';
    m_bitsetStr[10] = '0';

    m_interleave = 1;

    delete[] tmp;
  }
};

bool recodeImage ( const char  * srcFileName,
                   const char  * dstFileName,
                   int           interleave,
                   uint16_t      byteOrder )
{

  if(byteOrder != ENDIAN_LITTLE && byteOrder != ENDIAN_BIG){
    return false;
  }

  if (!strcmp(srcFileName, dstFileName)) {
    return false;
  }

  // interleave check
  if(interleave != 1 && interleave != 2
    && interleave != 4 && interleave != 8
    && interleave != 16 && interleave != 32
    && interleave != 64){
      //cout << "interleave neni mocnina 2" << endl;
      return false;
  }

  ifstream binFile;
  int size = 0;

  binFile.open(srcFileName, ios::in | ios::binary);
  if (!binFile.is_open()) {
    return false;
  }

  binFile.seekg(0, ios::end);
  size = binFile.tellg();

  // header size check
  if(size < 8){
    //cout << "mala hlavicka" << endl;
    binFile.close();
    return false;
  }
  char* buffer = new char[size];

  binFile.seekg(0, ios::beg);
  binFile.read(buffer, size);
  binFile.close();

  //cout << size << endl;
  CImage img(buffer, size);
  //cout << "------------------------------------" << endl;
  //cout << "Jmeno souboru: " << srcFileName << endl;
  //img.printInfo();

  // check validity
  if(!img.getIsValid()){
    delete[] buffer;
    return false;
  }

  //img.printInfo();
  //img.printPixels();
  img.revert();
  img.interleave(interleave);
  //cout << "------------------------------------" << endl;

  ofstream toBinFile(dstFileName, ios::out | ios::binary);
  if (!toBinFile.is_open()) {
    delete[] buffer;
    return false;
  }

  char* tmp = img.toString(byteOrder);
  toBinFile.write(tmp, size);
  if(!toBinFile.good()){

  }
  toBinFile.close();

  delete[] tmp;
  delete[] buffer;

  if(!toBinFile.good()){
    return false;
  }

  return true;
}

#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1,
                      const char * fileName2 )
{
  ifstream binFile;
  int refSize = 0;
  int outSize = 0;

  binFile.open(fileName1, ios::in | ios::binary);
  binFile.seekg(0, ios::end);
  refSize = binFile.tellg();
  char* refBuffer = new char[refSize];
  binFile.seekg(0, ios::beg);
  binFile.read(refBuffer, refSize);
  binFile.close();

  binFile.open(fileName2, ios::in | ios::binary);
  binFile.seekg(0, ios::end);
  outSize = binFile.tellg();
  char* outBuffer = new char[outSize];
  binFile.seekg(0, ios::beg);
  binFile.read(outBuffer, outSize);
  binFile.close();

  if (refSize != outSize) {
    delete[] refBuffer;
    delete[] outBuffer;
    return false;
  }

  if (strncmp(outBuffer, refBuffer, outSize)) {
    cout << !strncmp(outBuffer, refBuffer, outSize) << endl;
    delete[] refBuffer;
    delete[] outBuffer;
    return false;
  }

  delete[] refBuffer;
  delete[] outBuffer;
  return true;
}

int main ( void )
{
  assert ( recodeImage ( "test_data/input_00.img", "test_data/output_00.img", 1, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_00.img", "test_data/ref_00.img" ) );

  assert ( recodeImage ( "test_data/input_01.img", "test_data/output_01.img", 8, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_01.img", "test_data/ref_01.img" ) );

  assert ( recodeImage ( "test_data/input_02.img", "test_data/output_02.img", 8, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_02.img", "test_data/ref_02.img" ) );

  assert ( recodeImage ( "test_data/input_03.img", "test_data/output_03.img", 2, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_03.img", "test_data/ref_03.img" ) );

  assert ( recodeImage ( "test_data/input_04.img", "test_data/output_04.img", 1, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_04.img", "test_data/ref_04.img" ) );

  assert ( recodeImage ( "test_data/input_05.img", "test_data/output_05.img", 1, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_05.img", "test_data/ref_05.img" ) );

  assert ( recodeImage ( "test_data/input_06.img", "test_data/output_06.img", 8, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_06.img", "test_data/ref_06.img" ) );

  assert ( recodeImage ( "test_data/input_07.img", "test_data/output_07.img", 4, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_07.img", "test_data/ref_07.img" ) );

  assert ( recodeImage ( "test_data/input_08.img", "test_data/output_08.img", 8, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/output_08.img", "test_data/ref_08.img" ) );

  assert ( ! recodeImage ( "test_data/input_09.img", "test_data/output_09.img", 1, ENDIAN_LITTLE ) );

  assert ( ! recodeImage ( "test_data/input_10.img", "test_data/output_10.img", 5, ENDIAN_LITTLE ) );

  // extra inputs (optional & bonus tests)
  assert ( recodeImage ( "test_data/extra_input_00.img", "test_data/extra_out_00.img", 8, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/extra_out_00.img", "test_data/extra_ref_00.img" ) );
  assert ( recodeImage ( "test_data/extra_input_01.img", "test_data/extra_out_01.img", 4, ENDIAN_BIG )
           && identicalFiles ( "test_data/extra_out_01.img", "test_data/extra_ref_01.img" ) );
  assert ( recodeImage ( "test_data/extra_input_02.img", "test_data/extra_out_02.img", 16, ENDIAN_BIG )
           && identicalFiles ( "test_data/extra_out_02.img", "test_data/extra_ref_02.img" ) );
  assert ( recodeImage ( "test_data/extra_input_03.img", "test_data/extra_out_03.img", 1, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/extra_out_03.img", "test_data/extra_ref_03.img" ) );
  assert ( recodeImage ( "test_data/extra_input_04.img", "test_data/extra_out_04.img", 8, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/extra_out_04.img", "test_data/extra_ref_04.img" ) );
  assert ( recodeImage ( "test_data/extra_input_05.img", "test_data/extra_out_05.img", 4, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/extra_out_05.img", "test_data/extra_ref_05.img" ) );
  assert ( recodeImage ( "test_data/extra_input_06.img", "test_data/extra_out_06.img", 16, ENDIAN_BIG )
           && identicalFiles ( "test_data/extra_out_06.img", "test_data/extra_ref_06.img" ) );
  assert ( recodeImage ( "test_data/extra_input_07.img", "test_data/extra_out_07.img", 1, ENDIAN_BIG )
           && identicalFiles ( "test_data/extra_out_07.img", "test_data/extra_ref_07.img" ) );
  assert ( recodeImage ( "test_data/extra_input_08.img", "test_data/extra_out_08.img", 8, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/extra_out_08.img", "test_data/extra_ref_08.img" ) );
  assert ( recodeImage ( "test_data/extra_input_09.img", "test_data/extra_out_09.img", 4, ENDIAN_LITTLE )
           && identicalFiles ( "test_data/extra_out_09.img", "test_data/extra_ref_09.img" ) );
  // 1-bit bonus
  // assert ( recodeImage ( "test_data/extra_input_10.img", "test_data/extra_out_10.img", 16, ENDIAN_BIG )
  //          && identicalFiles ( "test_data/extra_out_10.img", "test_data/extra_ref_10.img" ) );
  // assert ( recodeImage ( "test_data/extra_input_11.img", "test_data/extra_out_11.img", 1, ENDIAN_BIG )
  //          && identicalFiles ( "test_data/extra_out_11.img", "test_data/extra_ref_11.img" ) );
  return 0;
}
#endif /* __PROGTEST__ */