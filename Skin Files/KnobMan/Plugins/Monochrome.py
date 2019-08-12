import sys
import ctypes

user32=ctypes.windll.user32
WM_SETTEXT=12

def KMSendText(hwnd,text):
  user32.SendMessageA(hwnd,WM_SETTEXT,0,text)

def KMGetText(hwnd):
  length = user32.GetWindowTextLengthA(hwnd)
  if length:
    buffer = ctypes.create_string_buffer("",length + 1)
    if user32.GetWindowTextA(hwnd, buffer, length +1):
      return buffer.value

def KMSetValue(hwnd,layer,param,value):
  s="KnobManCtl:SetValue "
  if layer==0:
    s=s+"Prefs."
  else:
    s=s+"Layer"+str(layer)+"."
  s=s+param+"="+str(value)
  KMSendText(hwnd,s)

def KMGetValue(hwnd,layer,param):
  s="KnobManCtl:GetValue "
  if layer==0:
    s=s+"Prefs."
  else:
    s=s+"Layer"+str(layer)+"."
  s=s+param
  KMSendText(hwnd,s)
  return KMGetText(hwnd)

def KMExport(hwnd,file):
  KMSendText(hwnd,"KnobManCtl:ExportImage "+file)

def KMExit(hwnd):
  KMSendText(hwnd,"KnobManCtl:Exit")


###################################################
def Main():
  hwnd=int(sys.argv[1])
  user32.MessageBoxA(0,"Change to Monochrome","Monochrome",0)
  layers=int(KMGetValue(hwnd,0,"Layers"))
  for layer in range(1,layers+1):
    KMSetValue(hwnd,layer,"Saturation1",-100)
    KMSetValue(hwnd,layer,"Saturation2",-100)
  KMExit(hwnd)
  user32.MessageBoxA(0,"Completed.","Monochrome",0)

Main()
