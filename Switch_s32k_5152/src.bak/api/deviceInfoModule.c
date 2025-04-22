#include <deviceInfoModule.h>
#include <string.h>

MSD_U16 deviceinfoGetDeviceNumAndRevByTypename(const char* deviceTypename)
{
    MSD_U16 value = 0;

    if (strcmp(deviceTypename, "88E6141") == 0)
        value = 0x3400;
    else if (strcmp(deviceTypename, "88E6341") == 0)
        value = 0x3410;
    else if (strcmp(deviceTypename, "88E6390X") == 0)
        value = 0x0A10;
    else if (strcmp(deviceTypename, "88E6390") == 0)
        value = 0x3900;
    else if (strcmp(deviceTypename, "88E6190X") == 0)
        value = 0x0A00;
    else if (strcmp(deviceTypename, "88E6190") == 0)
        value = 0x1900;
    else if (strcmp(deviceTypename, "88E6290") == 0)
        value = 0x2900;
    else if (strcmp(deviceTypename, "88E6352") == 0)
        value = 0x3520;
    else if (strcmp(deviceTypename, "88E6172") == 0)
        value = 0x1720;
    else if (strcmp(deviceTypename, "88E6176") == 0)
        value = 0x1760;
    else if (strcmp(deviceTypename, "88E6240") == 0)
        value = 0x2400;
    else if (strcmp(deviceTypename, "88E6320") == 0)
        value = 0x1150;
    else if (strcmp(deviceTypename, "88E6321") == 0)
        value = 0x3100;
    else if (strcmp(deviceTypename, "88E6191X") == 0)
        value = 0x1920;
    else if (strcmp(deviceTypename, "88E6393X") == 0)
        value = 0x3930;
    else if (strcmp(deviceTypename, "88E6193X") == 0)
        value = 0x1930;
    else if (strcmp(deviceTypename, "88E6391X") == 0)
        value = 0x3910;
    else if (strcmp(deviceTypename, "88Q5072") == 0)
        value = 0xA721;
    else if (strcmp(deviceTypename, "88Q6113") == 0)
        value = 0xF131;
    else if (strcmp(deviceTypename, "88Q5040Z1") == 0)
        value = 0xA402;
    else if (strcmp(deviceTypename, "88Q5151") == 0)
        value = 0xb510;
    else if (strcmp(deviceTypename, "88Q5152") == 0)
        value = 0xb520;
    else if (strcmp(deviceTypename, "88Q5192") == 0)
        value = 0xb920;
    else if (strcmp(deviceTypename, "88Q5040") == 0)
        value = 0x1240;
    else
        value = 0;

    return value;
}


char* deviceinfoGetDeviceNameByDeviceType(int deviceType)
{
    char* ret;
    switch (deviceType)
    {
    case MSD_88E6190:
    case MSD_88E6190X:
    case MSD_88E6390:
    case MSD_88E6390X:
    case MSD_88E6290:
        ret = "Peridot";
        break;

    case MSD_88E6141:
    case MSD_88E6341:
        ret = "Topaz";
        break;
    case MSD_88E6352:
    case MSD_88E6172:
    case MSD_88E6176:
    case MSD_88E6240:
        ret = "Agate";
        break;
    case MSD_88E6320:
    case MSD_88E6321:
        ret = "Pearl";
        break;
    case MSD_88E6191X:
    case MSD_88E6393X:
    case MSD_88E6193X:
    case MSD_88E6391X:
        ret = "Amethyst";
        break;
    case MSD_88Q5072:
        ret = "Oak";
        break;
    case MSD_88Q6113:
        ret = "Spruce";
        break;
    case MSD_88Q5040Z1:
        ret = "Bonsai-Z1";
        break;
    case MSD_88Q5151:
    case MSD_88Q5152:
    case MSD_88Q5192:
        ret = "Fir";
        break;
    case MSD_88Q5040:
        ret = "Bonsai";
        break;
    default:
        ret = "Unknown";
        break;
    }
    return ret;
}
