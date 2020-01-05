#include <QImage>
#include <Windows.h>
using namespace std;


class ImageKernel{

private:
    int width = 0;
    int height = 0;

    int * colorData;
    BYTE * derivData;
    int * tempColorData;
    BYTE * tempDerivData;
    QImage img;
    bool isForwardPass = true;

    int *runtype;

public:
    ImageKernel()
    {
        runtype = new int[3];//Kernel run type, unblur strength (0-255), refine strength (0-255)
        runtype[0] = 0;
        runtype[1] = 80;
        runtype[2] = 225;
    }
    void setBufferedImage(QImage &img) {

        if (width != img.width() || height != img.height()) {
            colorData = (int *)img.bits();
            derivData = new BYTE[img.width() * img.height()];
            tempColorData = new int[img.width() * img.height()];
            tempDerivData = new BYTE[img.width() * img.height()];

            width = img.width();
            height = img.height();
        }
        isForwardPass = true;
        this->img = img.copy();

    }

    void updateBufferedImage() {
        if (!isForwardPass) {
            runtype[0] = -1;
            this->execute(img.width() * img.height());
            isForwardPass = !isForwardPass;
        }
    }
    void execute(int range)
    {
        for (int i = 0; i < range; i++)
        {
            this->run(i);
        }
    }
    void setPushStrength(float strength) {
        runtype[1] = clamp((int)(strength * 255), 0, 0xFFFF);
    }

    void setPushGradStrength(float strength) {
        runtype[2] = clamp((int)(strength * 255), 0, 0xFFFF);
    }

    void process() {
        //Compute Lum
        runtype[0] = 0;
        this->execute(img.width() * img.height());

        int maxUnblur = runtype[1];
        int remainingUnblur = runtype[1];

        while (remainingUnblur > 0) { //While we still have to remove blur spread
            int currentThin = min(remainingUnblur, 0xFF);

            if (isForwardPass) {
                runtype[0] = 1;
            }
            else {
                runtype[0] = 2;
            }
            this->execute(img.width() * img.height());
            isForwardPass = !isForwardPass;

            remainingUnblur -= currentThin;
        }

        runtype[1] = maxUnblur; //Restore original value

                                //Compute Gradient
        if (isForwardPass) {
            runtype[0] = 3;
        }
        else {
            runtype[0] = 4;
        }
        this->execute(img.width() * img.height());
        isForwardPass = !isForwardPass;

        int maxRefine = runtype[2];
        int remainingRefine = runtype[2];

        while (remainingRefine > 0) { //While we still have to refine
            int currentRefine = min(remainingRefine, 0xFF);

            if (isForwardPass) {
                runtype[0] = 5;
            }
            else {
                runtype[0] = 6;
            }
            this->execute(img.width() * img.height());
            isForwardPass = !isForwardPass;

            remainingRefine -= currentRefine;
        }

        runtype[2] = maxRefine; //Restore original value



    }
private:
    int clamp(int i, int min, int max) {
        if (i < min) {
            i = min;
        }
        else if (i > max) {
            i = max;
        }

        return i;
    }
    int getAlpha(int argb) {
        return argb >> 24 & 0xFF;
    }
    int getRed(int rgb) {
        return rgb >> 16 & 0xFF;
    }
    int getGreen(int rgb) {
        return rgb >> 8 & 0xFF;
    }
    int getBlue(int rgb) {
        return rgb & 0xFF;
    }
    int toARGB(int a, int r, int g, int b) {
        return (clamp(a, 0, 0xFF) << 24) | (clamp(r, 0, 0xFF) << 16) | (clamp(g, 0, 0xFF) << 8) | (clamp(b, 0, 0xFF));
    }
    int getLuminance(int r, int g, int b) {
        return (r + r + g + g + g + b) / 6;
    }
    int unsignedByteToInt(byte b) {
        return ((int)b) & 0xFF;
    }

    bool compareLuminance3(int dark0, int dark1, int dark2, int light0, int light1, int light2) {
        return (dark0 < light0 && dark0 < light1 && dark0 < light2 && dark1 < light0 && dark1 < light1 && dark1 < light2 && dark2 < light0 && dark2 < light1 && dark2 < light2);
    }

    bool compareLuminance4(int dark0, int dark1, int dark2, int light0, int light1, int light2, int light3) {
        return (dark0 < light0 && dark0 < light1 && dark0 < light2 && dark0 < light3 && dark1 < light0 && dark1 < light1 && dark1 < light2&& dark1 < light3 && dark2 < light0 && dark2 < light1 && dark2 < light2 && dark2 < light3);
    }

    int averageGray(int d0, int d1, int d2) {
        return clamp((d0 + d1 + d2) / 3, 0, 0xFF);
    }

    int averageRGB(int c0, int c1, int c2) {
        int ra = (getRed(c0) + getRed(c1) + getRed(c2)) / 3;
        int ga = (getGreen(c0) + getGreen(c1) + getGreen(c2)) / 3;
        int ba = (getBlue(c0) + getBlue(c1) + getBlue(c2)) / 3;
        int aa = (getAlpha(c0) + getAlpha(c1) + getAlpha(c2)) / 3;

        return toARGB(aa, ra, ga, ba);
    }

    int weightedAverageGray(int d0, int d1, int alpha) {
        return clamp((d0 * (0xFF - alpha) + d1 * alpha) / 0xFF, 0, 0xFF);
    }

    int weightedAverageRGB(int c0, int c1, int alpha) {
        int ra = (getRed(c0) * (0xFF - alpha) + getRed(c1) * alpha) / 0xFF;
        int ga = (getGreen(c0) * (0xFF - alpha) + getGreen(c1) * alpha) / 0xFF;
        int ba = (getBlue(c0) * (0xFF - alpha) + getBlue(c1) * alpha) / 0xFF;
        int aa = (getAlpha(c0) * (0xFF - alpha) + getAlpha(c1) * alpha) / 0xFF;

        return toARGB(aa, ra, ga, ba);
    }

    void gradientRefine(int id, int strength, bool inversePass) {

        int * thisColorData = colorData;
        BYTE * thisDerivData = derivData;
        int * thisTempColorData = tempColorData;
        BYTE * thisTempDerivData = tempDerivData;

        if (inversePass) {
            thisColorData = tempColorData;
            thisDerivData = tempDerivData;
            thisTempColorData = colorData;
            thisTempDerivData = derivData;
        }

        strength = clamp(strength, 0, 0xFF);

        int x = id % width;
        int y = id / width;

        //Default translation constants
        int xn = -1;
        int xp = 1;
        int yn = -width;
        int yp = width;

        //If x or y is on the border, don't move out of bounds
        if (x == 0) {
            xn = 0;
        }
        else if (x == width - 1) {
            xp = 0;
        }
        if (y == 0) {
            yn = 0;
        }
        else if (y == height - 1) {
            yp = 0;
        }

        //Get indices for adjacent cells
        int ti = id + yn;
        int tli = ti + xn;
        int tri = ti + xp;

        int li = id + xn;
        int ri = id + xp;

        int bi = id + yp;
        int bli = bi + xn;
        int bri = bi + xp;

        int di0 = id;
        int di1 = id;
        int di2 = id;

        int li0 = id;
        int li1 = id;
        int li2 = id;
        int li3 = id;

        bool l4 = false;

        for (int kernelType = 0; kernelType<8; kernelType++) {
            if (kernelType == 0) { //Todo, binary search instead of searching entire list
                di0 = tli;
                di1 = ti;
                di2 = tri;

                li0 = id;
                li1 = bli;
                li2 = bi;
                li3 = bri;

                l4 = true;

            }
            else if (kernelType == 1) {
                di0 = ti;
                di1 = tri;
                di2 = ri;

                li0 = id;
                li1 = li;
                li2 = bi;

                l4 = false;

            }
            else if (kernelType == 2) {
                di0 = tri;
                di1 = ri;
                di2 = bri;

                li0 = id;
                li1 = tli;
                li2 = li;
                li3 = bli;

                l4 = true;

            }
            else if (kernelType == 3) {
                di0 = ri;
                di1 = bri;
                di2 = bi;

                li0 = id;
                li1 = ti;
                li2 = li;

                l4 = false;

            }
            else if (kernelType == 4) {
                di0 = bli;
                di1 = bi;
                di2 = bri;

                li0 = id;
                li1 = tli;
                li2 = ti;
                li3 = tri;

                l4 = true;

            }
            else if (kernelType == 5) {
                di0 = li;
                di1 = bli;
                di2 = bi;

                li0 = id;
                li1 = ti;
                li2 = ri;

                l4 = false;

            }
            else if (kernelType == 6) {
                di0 = tli;
                di1 = li;
                di2 = bli;

                li0 = id;
                li1 = tri;
                li2 = ri;
                li3 = bri;

                l4 = true;

            }
            else if (kernelType == 7) {
                di0 = tli;
                di1 = ti;
                di2 = li;

                li0 = id;
                li1 = bi;
                li2 = ri;

                l4 = false;

            }
            else if (kernelType < 0 || kernelType >= 8) {
                return;
            }

            if (l4) {

                int d0 = unsignedByteToInt(thisDerivData[di0]);
                int d1 = unsignedByteToInt(thisDerivData[di1]);
                int d2 = unsignedByteToInt(thisDerivData[di2]);

                int l0 = unsignedByteToInt(thisDerivData[li0]);
                int l1 = unsignedByteToInt(thisDerivData[li1]);
                int l2 = unsignedByteToInt(thisDerivData[li2]);
                int l3 = unsignedByteToInt(thisDerivData[li3]);

                if (compareLuminance4(d0, d1, d2, l0, l1, l2, l3)) {
                    int c0 = thisColorData[di0];
                    int c1 = thisColorData[di1];
                    int c2 = thisColorData[di2];
                    thisTempDerivData[id] = (byte)weightedAverageGray(thisDerivData[id], averageGray(d0, d1, d2), strength);
                    thisTempColorData[id] = weightedAverageRGB(thisColorData[id], averageRGB(c0, c1, c2), strength);
                    return;
                }
            }
            else {

                int d0 = unsignedByteToInt(thisDerivData[di0]);
                int d1 = unsignedByteToInt(thisDerivData[di1]);
                int d2 = unsignedByteToInt(thisDerivData[di2]);

                int l0 = unsignedByteToInt(thisDerivData[li0]);
                int l1 = unsignedByteToInt(thisDerivData[li1]);
                int l2 = unsignedByteToInt(thisDerivData[li2]);

                if (compareLuminance3(d0, d1, d2, l0, l1, l2)) {
                    int c0 = thisColorData[di0];
                    int c1 = thisColorData[di1];
                    int c2 = thisColorData[di2];
                    thisTempDerivData[id] = (byte)weightedAverageGray(thisDerivData[id], averageGray(d0, d1, d2), strength);
                    thisTempColorData[id] = weightedAverageRGB(thisColorData[id], averageRGB(c0, c1, c2), strength);
                    return;
                }
            }
        }
        thisTempDerivData[id] = thisDerivData[id];
        thisTempColorData[id] = thisColorData[id];


    }

    void unblur(int id, int strength, bool inversePass) {


        int * thisColorData = colorData;
        BYTE * thisDerivData = derivData;
        int * thisTempColorData = tempColorData;
        BYTE * thisTempDerivData = tempDerivData;

        if (inversePass) {
            thisColorData = tempColorData;
            thisDerivData = tempDerivData;
            thisTempColorData = colorData;
            thisTempDerivData = derivData;
        }

        strength = clamp(strength, 0, 0xFF);

        int x = id % width;
        int y = id / width;

        //Default translation constants
        int xn = -1;
        int xp = 1;
        int yn = -width;
        int yp = width;

        //If x or y is on the border, don't move out of bounds
        if (x == 0) {
            xn = 0;
        }
        else if (x == width - 1) {
            xp = 0;
        }
        if (y == 0) {
            yn = 0;
        }
        else if (y == height - 1) {
            yp = 0;
        }

        //Get indices for adjacent cells
        int ti = id + yn;
        int tli = ti + xn;
        int tri = ti + xp;

        int li = id + xn;
        int ri = id + xp;

        int bi = id + yp;
        int bli = bi + xn;
        int bri = bi + xp;

        int di0 = id;
        int di1 = id;
        int di2 = id;

        int li0 = id;
        int li1 = id;
        int li2 = id;
        int li3 = id;

        bool l4 = false;

        int lightestColor = thisColorData[id];
        int lightestLum = thisDerivData[id];

        for (int kernelType = 0; kernelType<8; kernelType++) {
            if (kernelType == 0) { //Todo, binary search instead of searching entire list
                di0 = tli;
                di1 = ti;
                di2 = tri;

                li0 = id;
                li1 = bli;
                li2 = bi;
                li3 = bri;

                l4 = true;

            }
            else if (kernelType == 1) {
                di0 = ti;
                di1 = tri;
                di2 = ri;

                li0 = id;
                li1 = li;
                li2 = bi;

                l4 = false;

            }
            else if (kernelType == 2) {
                di0 = tri;
                di1 = ri;
                di2 = bri;

                li0 = id;
                li1 = tli;
                li2 = li;
                li3 = bli;

                l4 = true;

            }
            else if (kernelType == 3) {
                di0 = ri;
                di1 = bri;
                di2 = bi;

                li0 = id;
                li1 = ti;
                li2 = li;

                l4 = false;

            }
            else if (kernelType == 4) {
                di0 = bli;
                di1 = bi;
                di2 = bri;

                li0 = id;
                li1 = tli;
                li2 = ti;
                li3 = tri;

                l4 = true;

            }
            else if (kernelType == 5) {
                di0 = li;
                di1 = bli;
                di2 = bi;

                li0 = id;
                li1 = ti;
                li2 = ri;

                l4 = false;

            }
            else if (kernelType == 6) {
                di0 = tli;
                di1 = li;
                di2 = bli;

                li0 = id;
                li1 = tri;
                li2 = ri;
                li3 = bri;

                l4 = true;

            }
            else if (kernelType == 7) {
                di0 = tli;
                di1 = ti;
                di2 = li;

                li0 = id;
                li1 = bi;
                li2 = ri;

                l4 = false;

            }
            else if (kernelType < 0 || kernelType >= 8) {
                return;
            }



            if (l4) {

                int d0 = unsignedByteToInt(thisDerivData[di0]);
                int d1 = unsignedByteToInt(thisDerivData[di1]);
                int d2 = unsignedByteToInt(thisDerivData[di2]);

                int l0 = unsignedByteToInt(thisDerivData[li0]);
                int l1 = unsignedByteToInt(thisDerivData[li1]);
                int l2 = unsignedByteToInt(thisDerivData[li2]);
                int l3 = unsignedByteToInt(thisDerivData[li3]);

                if (!compareLuminance4(d0, d1, d2, l0, l1, l2, l3)) {
                    int c0 = thisColorData[di0];
                    int c1 = thisColorData[di1];
                    int c2 = thisColorData[di2];

                    int newColor = weightedAverageRGB(thisColorData[id], averageRGB(c0, c1, c2), strength);
                    int newLum = getLuminance(getRed(newColor), getGreen(newColor), getBlue(newColor));

                    if (newLum > lightestLum) {
                        lightestLum = newLum;
                        lightestColor = newColor;
                    }

                }
            }
            else {

                int d0 = unsignedByteToInt(thisDerivData[di0]);
                int d1 = unsignedByteToInt(thisDerivData[di1]);
                int d2 = unsignedByteToInt(thisDerivData[di2]);

                int l0 = unsignedByteToInt(thisDerivData[li0]);
                int l1 = unsignedByteToInt(thisDerivData[li1]);
                int l2 = unsignedByteToInt(thisDerivData[li2]);

                if (!compareLuminance3(d0, d1, d2, l0, l1, l2)) {
                    int c0 = thisColorData[di0];
                    int c1 = thisColorData[di1];
                    int c2 = thisColorData[di2];

                    int newColor = weightedAverageRGB(thisColorData[id], averageRGB(c0, c1, c2), strength);
                    int newLum = getLuminance(getRed(newColor), getGreen(newColor), getBlue(newColor));

                    if (newLum > lightestLum) {
                        lightestLum = newLum;
                        lightestColor = newColor;
                    }
                }
            }
        }
        thisTempColorData[id] = lightestColor;
        thisTempDerivData[id] = (byte)clamp(lightestLum, 0, 0xFF);

    }

public:
        void computeGradient(int id, bool inversePass) {

        int * thisColorData = colorData;
        BYTE * thisDerivData = derivData;
        int * thisTempColorData = tempColorData;
        BYTE * thisTempDerivData = tempDerivData;

        if (inversePass) {
            thisColorData = tempColorData;
            thisDerivData = tempDerivData;
            thisTempColorData = colorData;
            thisTempDerivData = derivData;
        }

        int x = id % width;
        int y = id / width;

        //Default translation constants
        int xn = -1;
        int xp = 1;
        int yn = -width;
        int yp = width;

        //If x or y is on the border, don't move out of bounds
        if (x == 0) {
            xn = 0;
        }
        else if (x == width - 1) {
            xp = 0;
        }
        if (y == 0) {
            yn = 0;
        }
        else if (y == height - 1) {
            yp = 0;
        }

        //Get indices for adjacent cells
        int topi = id + yn;
        int topLefti = topi + xn;
        int topRighti = topi + xp;

        int lefti = id + xn;
        int righti = id + xp;

        int bottomi = id + yp;
        int bottomLefti = bottomi + xn;
        int bottomRighti = bottomi + xp;

        //Get values for adjacent cells
        int top = ((int)thisDerivData[topi]) & 0xFF;
        int topLeft = ((int)thisDerivData[topLefti]) & 0xFF;
        int topRight = ((int)thisDerivData[topRighti]) & 0xFF;

        int left = ((int)thisDerivData[lefti]) & 0xFF;
        int right = ((int)thisDerivData[righti]) & 0xFF;

        int bottom = ((int)thisDerivData[bottomi]) & 0xFF;
        int bottomLeft = ((int)thisDerivData[bottomLefti]) & 0xFF;
        int bottomRight = ((int)thisDerivData[bottomRighti]) & 0xFF;

        //Perform Sobel Operation
        int xSobel = abs(-topLeft + topRight
            - left - left + right + right
            - bottomLeft + bottomRight);
        int ySobel = abs(-topLeft - top - top - topRight
            + bottomLeft + bottom + bottom + bottomRight);


        int deriv = clamp((xSobel + ySobel) / 2, 0, 0xFF);
        thisTempDerivData[id] = (byte)deriv;
        thisTempColorData[id] = thisColorData[id];
    }



    void run(int ID) {
        int id = ID;

        if (runtype[0] == 0) {

            int rgb = colorData[id];
            int r = getRed(rgb);
            int g = getGreen(rgb);
            int b = getBlue(rgb);
            int lum = clamp(getLuminance(r, g, b), 0, 0xFF);
            derivData[id] = (byte)lum;

        }
        else if (runtype[0] == 1) {
            unblur(id, runtype[1], false);
        }
        else if (runtype[0] == 2) {
            unblur(id, runtype[1], true);
        }
        else if (runtype[0] == 3) {
            computeGradient(id, false);
        }
        else if (runtype[0] == 4) {
            computeGradient(id, true);
        }
        else if (runtype[0] == 5) {
            gradientRefine(id, runtype[2], false);
        }
        else if (runtype[0] == 6) {
            gradientRefine(id, runtype[2], true);


        }
        else if (runtype[0] < 0) { //Prevent JVM from optimizing this chain of ifs into a switch statement, since switch/case causes Aparapi to crash
            derivData[id] = tempDerivData[id];
            colorData[id] = tempColorData[id];
        }

    }
};
