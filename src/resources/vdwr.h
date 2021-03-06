#ifndef VDWR_H_
#define VDWR_H_

namespace resources
{

unsigned char vdwr_classes[] = {
  0x43, 0x41, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x31, 0x2e, 0x39, 0x30, 0x0a, 0x43, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x37, 0x35, 0x0a, 0x43, 0x48,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x32, 0x2e,
  0x30, 0x31, 0x0a, 0x43, 0x48, 0x32, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x31, 0x2e, 0x39, 0x32, 0x0a, 0x43, 0x48, 0x32, 0x62,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x39, 0x31,
  0x0a, 0x43, 0x48, 0x32, 0x63, 0x68, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x31, 0x2e, 0x38, 0x38, 0x0a, 0x43, 0x48, 0x33, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x39, 0x32, 0x0a, 0x43,
  0x48, 0x61, 0x72, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31,
  0x2e, 0x38, 0x32, 0x0a, 0x43, 0x61, 0x72, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x37, 0x34, 0x0a, 0x43, 0x48, 0x69,
  0x6d, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x37,
  0x34, 0x0a, 0x43, 0x63, 0x6f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x31, 0x2e, 0x38, 0x31, 0x0a, 0x43, 0x63, 0x6f, 0x6f, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x37, 0x36, 0x0a,
  0x53, 0x48, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x31, 0x2e, 0x38, 0x38, 0x0a, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x39, 0x34, 0x0a, 0x4e, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e,
  0x37, 0x30, 0x0a, 0x4e, 0x48, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x31, 0x2e, 0x36, 0x36, 0x0a, 0x4e, 0x48, 0x2b, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x36, 0x35,
  0x0a, 0x4e, 0x48, 0x32, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x31, 0x2e, 0x36, 0x32, 0x0a, 0x4e, 0x48, 0x32, 0x2b, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x36, 0x37, 0x0a, 0x4e,
  0x48, 0x33, 0x2b, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31,
  0x2e, 0x36, 0x37, 0x0a, 0x4f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x34, 0x39, 0x0a, 0x4f, 0x63, 0x6f,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x35,
  0x32, 0x0a, 0x4f, 0x63, 0x6f, 0x6f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x31, 0x2e, 0x34, 0x39, 0x0a, 0x4f, 0x48, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x35, 0x34, 0x0a,
  0x48, 0x32, 0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x31, 0x2e, 0x36, 0x38, 0x0a, 0x0a, 0x43, 0x4c, 0x5f, 0x64, 0x65, 0x66,
  0x61, 0x75, 0x6c, 0x74, 0x20, 0x20, 0x31, 0x2e, 0x38, 0x30, 0x0a, 0x43,
  0x5f, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x20, 0x20, 0x20, 0x31,
  0x2e, 0x38, 0x30, 0x0a, 0x46, 0x45, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75,
  0x6c, 0x74, 0x20, 0x20, 0x31, 0x2e, 0x38, 0x30, 0x0a, 0x46, 0x5f, 0x64,
  0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x38,
  0x30, 0x0a, 0x4e, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x20,
  0x20, 0x20, 0x31, 0x2e, 0x36, 0x30, 0x0a, 0x4f, 0x5f, 0x64, 0x65, 0x66,
  0x61, 0x75, 0x6c, 0x74, 0x20, 0x20, 0x20, 0x31, 0x2e, 0x35, 0x30, 0x0a,
  0x53, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x20, 0x20, 0x20,
  0x31, 0x2e, 0x39, 0x30, 0x0a, 0x5a, 0x4e, 0x5f, 0x64, 0x65, 0x66, 0x61,
  0x75, 0x6c, 0x74, 0x20, 0x20, 0x31, 0x2e, 0x37, 0x30, 0x0a, 0x0a, 0x64,
  0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31,
  0x2e, 0x38, 0x30, 0x0a
};
unsigned int vdwr_classes_len = 580;

unsigned char vdwr_members[] = {
  0x2a, 0x5f, 0x43, 0x41, 0x09, 0x09, 0x09, 0x43, 0x41, 0x0a, 0x0a, 0x2a,
  0x5f, 0x43, 0x09, 0x09, 0x09, 0x09, 0x43, 0x0a, 0x0a, 0x49, 0x4c, 0x45,
  0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x0a, 0x4c, 0x45, 0x55,
  0x5f, 0x43, 0x47, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x0a, 0x54, 0x48,
  0x52, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x0a, 0x56, 0x41,
  0x4c, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x0a, 0x0a, 0x41,
  0x52, 0x47, 0x5f, 0x43, 0x47, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32,
  0x0a, 0x49, 0x4c, 0x45, 0x5f, 0x43, 0x47, 0x31, 0x09, 0x09, 0x09, 0x43,
  0x48, 0x32, 0x0a, 0x4c, 0x59, 0x53, 0x5f, 0x43, 0x47, 0x2a, 0x09, 0x09,
  0x09, 0x43, 0x48, 0x32, 0x0a, 0x4c, 0x59, 0x53, 0x5f, 0x43, 0x44, 0x2a,
  0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x0a, 0x4d, 0x45, 0x54, 0x5f, 0x43,
  0x47, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x0a, 0x50, 0x52, 0x4f,
  0x5f, 0x43, 0x47, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x0a, 0x50,
  0x52, 0x4f, 0x5f, 0x43, 0x44, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32,
  0x0a, 0x0a, 0x41, 0x52, 0x47, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43,
  0x48, 0x32, 0x62, 0x0a, 0x41, 0x53, 0x4e, 0x5f, 0x43, 0x42, 0x09, 0x09,
  0x09, 0x43, 0x48, 0x32, 0x62, 0x0a, 0x41, 0x53, 0x50, 0x5f, 0x43, 0x42,
  0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x62, 0x0a, 0x43, 0x59, 0x53, 0x5f,
  0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x62, 0x0a, 0x47, 0x4c,
  0x4e, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x62, 0x0a,
  0x47, 0x4c, 0x55, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32,
  0x62, 0x0a, 0x48, 0x49, 0x53, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43,
  0x48, 0x32, 0x62, 0x0a, 0x4c, 0x45, 0x55, 0x5f, 0x43, 0x42, 0x09, 0x09,
  0x09, 0x43, 0x48, 0x32, 0x62, 0x0a, 0x4c, 0x59, 0x53, 0x5f, 0x43, 0x42,
  0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x62, 0x0a, 0x4d, 0x45, 0x54, 0x5f,
  0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x62, 0x0a, 0x50, 0x48,
  0x45, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x62, 0x0a,
  0x50, 0x52, 0x4f, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32,
  0x62, 0x0a, 0x53, 0x45, 0x52, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43,
  0x48, 0x32, 0x62, 0x0a, 0x54, 0x52, 0x50, 0x5f, 0x43, 0x42, 0x09, 0x09,
  0x09, 0x43, 0x48, 0x32, 0x62, 0x0a, 0x54, 0x59, 0x52, 0x5f, 0x43, 0x42,
  0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x62, 0x0a, 0x0a, 0x41, 0x52, 0x47,
  0x5f, 0x43, 0x44, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x63, 0x68,
  0x0a, 0x47, 0x4c, 0x55, 0x5f, 0x43, 0x47, 0x2a, 0x09, 0x09, 0x09, 0x43,
  0x48, 0x32, 0x63, 0x68, 0x0a, 0x4c, 0x59, 0x53, 0x5f, 0x43, 0x45, 0x2a,
  0x09, 0x09, 0x09, 0x43, 0x48, 0x32, 0x63, 0x68, 0x0a, 0x0a, 0x41, 0x4c,
  0x41, 0x5f, 0x43, 0x42, 0x09, 0x09, 0x09, 0x43, 0x48, 0x33, 0x0a, 0x49,
  0x4c, 0x45, 0x5f, 0x43, 0x47, 0x32, 0x09, 0x09, 0x09, 0x43, 0x48, 0x33,
  0x0a, 0x49, 0x4c, 0x45, 0x5f, 0x43, 0x44, 0x31, 0x09, 0x09, 0x09, 0x43,
  0x48, 0x33, 0x0a, 0x4c, 0x45, 0x55, 0x5f, 0x43, 0x44, 0x31, 0x09, 0x09,
  0x09, 0x43, 0x48, 0x33, 0x0a, 0x4c, 0x45, 0x55, 0x5f, 0x43, 0x44, 0x32,
  0x09, 0x09, 0x09, 0x43, 0x48, 0x33, 0x0a, 0x54, 0x48, 0x52, 0x5f, 0x43,
  0x47, 0x32, 0x09, 0x09, 0x09, 0x43, 0x48, 0x33, 0x0a, 0x56, 0x41, 0x4c,
  0x5f, 0x43, 0x47, 0x31, 0x09, 0x09, 0x09, 0x43, 0x48, 0x33, 0x0a, 0x56,
  0x41, 0x4c, 0x5f, 0x43, 0x47, 0x32, 0x09, 0x09, 0x09, 0x43, 0x48, 0x33,
  0x0a, 0x0a, 0x50, 0x48, 0x45, 0x5f, 0x43, 0x44, 0x2a, 0x09, 0x09, 0x09,
  0x43, 0x48, 0x61, 0x72, 0x0a, 0x50, 0x48, 0x45, 0x5f, 0x43, 0x45, 0x2a,
  0x09, 0x09, 0x09, 0x43, 0x48, 0x61, 0x72, 0x0a, 0x50, 0x48, 0x45, 0x5f,
  0x43, 0x5a, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x61, 0x72, 0x0a, 0x54,
  0x52, 0x50, 0x5f, 0x43, 0x44, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x61,
  0x72, 0x0a, 0x54, 0x52, 0x50, 0x5f, 0x43, 0x45, 0x2a, 0x09, 0x09, 0x09,
  0x43, 0x48, 0x61, 0x72, 0x0a, 0x54, 0x52, 0x50, 0x5f, 0x43, 0x5a, 0x2a,
  0x09, 0x09, 0x09, 0x43, 0x48, 0x61, 0x72, 0x0a, 0x54, 0x52, 0x50, 0x5f,
  0x43, 0x48, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x61, 0x72, 0x0a, 0x54,
  0x59, 0x52, 0x5f, 0x43, 0x44, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x61,
  0x72, 0x0a, 0x54, 0x59, 0x52, 0x5f, 0x43, 0x45, 0x2a, 0x09, 0x09, 0x09,
  0x43, 0x48, 0x61, 0x72, 0x0a, 0x0a, 0x50, 0x48, 0x45, 0x5f, 0x43, 0x47,
  0x2a, 0x09, 0x09, 0x09, 0x43, 0x61, 0x72, 0x0a, 0x54, 0x52, 0x50, 0x5f,
  0x43, 0x47, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x61, 0x72, 0x0a, 0x54, 0x52,
  0x50, 0x5f, 0x43, 0x45, 0x32, 0x09, 0x09, 0x09, 0x43, 0x61, 0x72, 0x0a,
  0x54, 0x59, 0x52, 0x5f, 0x43, 0x47, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x61,
  0x72, 0x0a, 0x0a, 0x48, 0x49, 0x53, 0x5f, 0x43, 0x44, 0x2a, 0x09, 0x09,
  0x09, 0x43, 0x48, 0x69, 0x6d, 0x0a, 0x48, 0x49, 0x53, 0x5f, 0x43, 0x45,
  0x2a, 0x09, 0x09, 0x09, 0x43, 0x48, 0x69, 0x6d, 0x0a, 0x0a, 0x41, 0x53,
  0x4e, 0x5f, 0x43, 0x47, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x63, 0x6f, 0x0a,
  0x47, 0x4c, 0x4e, 0x5f, 0x43, 0x44, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x63,
  0x6f, 0x0a, 0x0a, 0x41, 0x53, 0x50, 0x5f, 0x43, 0x47, 0x2a, 0x09, 0x09,
  0x09, 0x43, 0x63, 0x6f, 0x6f, 0x0a, 0x47, 0x4c, 0x55, 0x5f, 0x43, 0x44,
  0x2a, 0x09, 0x09, 0x09, 0x43, 0x63, 0x6f, 0x6f, 0x0a, 0x0a, 0x43, 0x59,
  0x53, 0x5f, 0x53, 0x2a, 0x09, 0x09, 0x09, 0x53, 0x48, 0x0a, 0x0a, 0x4d,
  0x45, 0x54, 0x5f, 0x53, 0x2a, 0x09, 0x09, 0x09, 0x53, 0x0a, 0x0a, 0x2a,
  0x5f, 0x4e, 0x09, 0x09, 0x09, 0x09, 0x4e, 0x0a, 0x0a, 0x54, 0x52, 0x50,
  0x5f, 0x4e, 0x45, 0x31, 0x09, 0x09, 0x09, 0x4e, 0x48, 0x0a, 0x0a, 0x48,
  0x49, 0x53, 0x5f, 0x4e, 0x45, 0x31, 0x09, 0x09, 0x09, 0x4e, 0x48, 0x2b,
  0x0a, 0x48, 0x49, 0x53, 0x5f, 0x4e, 0x44, 0x32, 0x09, 0x09, 0x09, 0x4e,
  0x48, 0x2b, 0x0a, 0x0a, 0x41, 0x53, 0x4e, 0x5f, 0x4e, 0x44, 0x32, 0x09,
  0x09, 0x09, 0x4e, 0x48, 0x32, 0x0a, 0x47, 0x4c, 0x4e, 0x5f, 0x4e, 0x45,
  0x32, 0x09, 0x09, 0x09, 0x4e, 0x48, 0x32, 0x0a, 0x41, 0x52, 0x47, 0x5f,
  0x4e, 0x45, 0x2a, 0x09, 0x09, 0x09, 0x4e, 0x48, 0x32, 0x0a, 0x41, 0x52,
  0x47, 0x5f, 0x4e, 0x48, 0x31, 0x09, 0x09, 0x09, 0x4e, 0x48, 0x32, 0x0a,
  0x0a, 0x41, 0x52, 0x47, 0x5f, 0x4e, 0x48, 0x32, 0x09, 0x09, 0x09, 0x4e,
  0x48, 0x32, 0x2b, 0x0a, 0x0a, 0x4c, 0x59, 0x53, 0x5f, 0x4e, 0x5a, 0x2a,
  0x09, 0x09, 0x09, 0x4e, 0x48, 0x33, 0x2b, 0x0a, 0x0a, 0x2a, 0x5f, 0x4f,
  0x09, 0x09, 0x09, 0x09, 0x4f, 0x0a, 0x0a, 0x41, 0x53, 0x4e, 0x5f, 0x4f,
  0x44, 0x31, 0x09, 0x09, 0x09, 0x4f, 0x63, 0x6f, 0x0a, 0x47, 0x4c, 0x4e,
  0x5f, 0x4f, 0x45, 0x31, 0x09, 0x09, 0x09, 0x4f, 0x63, 0x6f, 0x0a, 0x0a,
  0x41, 0x53, 0x50, 0x5f, 0x4f, 0x44, 0x31, 0x09, 0x09, 0x09, 0x4f, 0x63,
  0x6f, 0x6f, 0x0a, 0x41, 0x53, 0x50, 0x5f, 0x4f, 0x44, 0x32, 0x09, 0x09,
  0x09, 0x4f, 0x63, 0x6f, 0x6f, 0x0a, 0x47, 0x4c, 0x55, 0x5f, 0x4f, 0x45,
  0x31, 0x09, 0x09, 0x09, 0x4f, 0x63, 0x6f, 0x6f, 0x0a, 0x47, 0x4c, 0x55,
  0x5f, 0x4f, 0x45, 0x32, 0x09, 0x09, 0x09, 0x4f, 0x63, 0x6f, 0x6f, 0x0a,
  0x0a, 0x53, 0x45, 0x52, 0x5f, 0x4f, 0x47, 0x2a, 0x09, 0x09, 0x09, 0x4f,
  0x48, 0x0a, 0x54, 0x48, 0x52, 0x5f, 0x4f, 0x47, 0x2a, 0x09, 0x09, 0x09,
  0x4f, 0x48, 0x0a, 0x54, 0x59, 0x52, 0x5f, 0x4f, 0x48, 0x2a, 0x09, 0x09,
  0x09, 0x4f, 0x48, 0x0a, 0x0a, 0x2a, 0x5f, 0x43, 0x4c, 0x2a, 0x09, 0x09,
  0x09, 0x43, 0x4c, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x0a,
  0x2a, 0x5f, 0x43, 0x2a, 0x09, 0x09, 0x09, 0x43, 0x5f, 0x64, 0x65, 0x66,
  0x61, 0x75, 0x6c, 0x74, 0x0a, 0x2a, 0x5f, 0x46, 0x45, 0x2a, 0x09, 0x09,
  0x09, 0x46, 0x45, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x0a,
  0x2a, 0x5f, 0x46, 0x2a, 0x09, 0x09, 0x09, 0x46, 0x5f, 0x64, 0x65, 0x66,
  0x61, 0x75, 0x6c, 0x74, 0x0a, 0x2a, 0x5f, 0x4e, 0x2a, 0x09, 0x09, 0x09,
  0x4e, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x0a, 0x2a, 0x5f,
  0x4f, 0x2a, 0x09, 0x09, 0x09, 0x4f, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75,
  0x6c, 0x74, 0x0a, 0x2a, 0x5f, 0x53, 0x2a, 0x09, 0x09, 0x09, 0x53, 0x5f,
  0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x0a, 0x2a, 0x5f, 0x5a, 0x4e,
  0x2a, 0x09, 0x09, 0x09, 0x5a, 0x4e, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75,
  0x6c, 0x74, 0x0a, 0x0a, 0x2a, 0x5f, 0x2a, 0x09, 0x09, 0x09, 0x09, 0x64,
  0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x0a
};
unsigned int vdwr_members_len = 1291;

}

#endif /* VDWR_H_ */
