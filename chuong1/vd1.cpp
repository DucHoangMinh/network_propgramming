#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<bits/stdc++.h>
using namespace std;

// VD1: Cho mảng chứa nội dung là header của gói tin IP hãy in ra giá trị version, ihl, total_length, địa chỉ IP nguồn và đích.

int main(){
    uint8_t header[] = {   
        0x45, 0x00, 0x00, 0x40,
        0x7c, 0xda, 0x40, 0x00,                         
        0x80, 0x06, 0xfa, 0xd8,                        
        0xc0, 0xa8, 0x0f, 0x0b,                         
        0xbc, 0xac, 0xf6, 0xa4};
    // Dich phai de lay 4 bit dau
    int version = (header[0] >> 4) & 0x0F;
    printf("Version: %d\n", version);
    //Ihl tinh bang so tu nho 32 bits, hay tinh bang so tu nho 4 bytes
    int ihl = (header[0] << 4);
    printf("Ihl : %d bytes\n", ihl * 4);
    //Total length tinh theo don vi bytes
    int total_length = header[2]*256 + header[3];
    printf("Total length %d\n", total_length);
    printf("Source IP: %d.%d.%d.%d\n", 
        (unsigned int)header[12], (unsigned char)header[13], (unsigned char)header[14], (unsigned char)header[15]);
    printf("Dest IP: %d.%d.%d.%d\n", 
        (unsigned char)header[16], (unsigned char)header[17], (unsigned char)header[18], (unsigned char)header[19]);
    cout << "END";
    return 0;
}