/* src/math_utils.c */
/* src/math_utils.c - Math helper functions (arctan, sqrt, sin/cos, vectors,
 * distance) */
#include "config.h"
#include "engine.h"

// math functions
float arctan_approx(float y, float x) {
  if (x == 0) {
    return (y > 0) ? 90.0 : (y < 0) ? 270.0 : 0.0;
  }

  float abs_y = (y < 0) ? -y : y;
  float abs_x = (x < 0) ? -x : x;
  float angle = 0.0;

  if (abs_x > abs_y) {
    float ratio = abs_y / abs_x;
    angle = ratio * 45.0;
  } else {
    float ratio = abs_x / abs_y;
    angle = 90.0 - (ratio * 45.0);
  }

  // Adjust quadrant
  if (x < 0 && y >= 0)
    return 180.0 - angle;
  if (x < 0 && y < 0)
    return 180.0 + angle;
  if (x >= 0 && y < 0)
    return 360.0 - angle;
  return angle;
}
float abs_(float x) {
  if (x < 0) {
    return -x;
  } else {
    return x;
  }
}
int abs_int(int x) {
  if (x < 0) {
    return -x;
  } else {
    return x;
  }
}
double myPow(double base, int exponent) {
  double result = 1.0;

  if (exponent > 0) {
    for (int i = 0; i < exponent; i++) {
      result *= base;
    }
  } else if (exponent < 0) {
    for (int i = 0; i > exponent; i--) {
      result /= base;
    }
  }

  return result;
}
double factorial(int n) {
  double fact = 1.0;
  for (int i = 1; i <= n; i++) {
    fact *= i;
  }
  return fact;
}
double mysqrt(double x) {
  double epsilon = 0.0001;
  double guess = 1.0;

  while (1) {
    double new_guess = 0.5 * (guess + x / guess);
    if (abs_(new_guess - guess) < epsilon) {
      break;
    }
    guess = new_guess;
  }
  return guess;
}
int integer_sqrt(int x) {
  int result = 0;
  int bit = 1 << 30; // The highest bit in a 32-bit integer

  // Find the highest bit that is set
  while (bit > x)
    bit >>= 2;

  // Newton's method for integer square root
  while (bit != 0) {
    if (x >= result + bit) {
      x -= result + bit;
      result = (result >> 1) + bit;
    } else {
      result >>= 1;
    }
    bit >>= 2;
  }

  return result;
}
int integer_sqrt1000(int n) {
  n *= 1000;

  int x = n;
  int y = (x + 1) / 2;
  while (y < x) {
    x = y;
    y = (x + n / x) / 2;
  }
  return x;
}
double normalizeAngle(double x) {
  const double pi = M_PI;
  while (x > pi) {
    x -= 2 * pi;
  }
  while (x < -pi) {
    x += 2 * pi;
  }
  return x;
}
double mySine(double x) {
  x = normalizeAngle(x); // Normalize the angle to [-pi, pi]

  double result = x;
  int sign = -1;
  for (int i = 3; i <= 15; i += 2) { // Adjust the range of the Taylor series
    result += sign * (myPow(x, i) / factorial(i));
    sign *= -1;
  }
  return result;
}
double myCosine(double x) {
  x = normalizeAngle(x); // Normalize the angle to [-pi, pi]

  double result = 1.0;
  int sign = -1;
  for (int i = 2; i <= 14; i += 2) { // Adjust the range of the Taylor series
    result += sign * (myPow(x, i) / factorial(i));
    sign *= -1;
  }
  return result;
}
double mytanges(double x) {
  x = normalizeAngle(x); // Normalize the angle to [-pi, pi]

  double result = 0.0;
  double term = x;
  int n = 5;

  for (int i = 1; i <= n; i++) {
    if (i % 2 == 0) {
      result -= term / i;
    } else {
      result += term / i;
    }
    term *= -x * x;
  }

  return result;
}
float ToRadians(float val) {
  return (M_PI / 180) * val; // PI / 180
}
int clamp(int value, int min, int max) {
  if (value < min) {
    return min;
  } else if (value > max) {
    return max;
  } else {
    return value;
  }
}
float fmodf_f(float x, float y) { return (float)(x - y * (int)(x / y)); }
color_t combineColors(color_t color1, color_t color2, int strength) {
  int strenthInverted = 100 - strength;
  char red1 = (color1 >> 11) & 0x1F;
  char green1 = (color1 >> 5) & 0x3F;
  char blue1 = color1 & 0x1F;

  red1 = (char)(red1 * strenthInverted / 100);
  green1 = (char)(green1 * strenthInverted / 100);
  blue1 = (char)(blue1 * strenthInverted / 100);

  char red2 = (color2 >> 11) & 0x1F;
  char green2 = (color2 >> 5) & 0x3F;
  char blue2 = color2 & 0x1F;

  red2 = (char)(red2 * strength / 100);
  green2 = (char)(green2 * strength / 100);
  blue2 = (char)(blue2 * strength / 100);

  color_t combinedColor =
      ((red1 + red2) << 11) | ((green1 + green2) << 5) | (blue1 + blue2);

  return combinedColor;
}
color_t combineColorsWithBrightness(color_t color, int brightness) {
  // Extract individual color components for color1
  unsigned int r1 = (color >> 11) & 0x1F;
  unsigned int g1 = (color >> 5) & 0x3F;
  unsigned int b1 = color & 0x1F;

  // Interpolate between color1 and color2 using the specified strength
  unsigned int r =
      (r1 << 4) * brightness >> 8; // r1 * 256 / 16 * brightness / 256
  unsigned int g = (g1 << 4) * brightness >> 8;
  unsigned int b = (b1 << 4) * brightness >> 8;

  // Combine the interpolated color components into a single RGB565 value
  return (r << 11) | (g << 5) | b;
}
float dotVec3(Vector3 a, Vector3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
int dotVec3I(Vector3I a, Vector3I b) {
  return a.x * b.x / 1000 + a.y * b.y / 1000 + a.z * b.z / 1000;
}
int dotVec3IND(Vector3I a, Vector3I b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
int crossVec2I(Vector2I a, Vector2I b, int precision) {
  return (a.x * b.y - a.y * b.x) / precision;
}
int crossVec2S(Vector2S a, Vector2S b, int precision) {
  return (a.x * b.y - a.y * b.x) / precision;
}
int crossVec2INP(Vector2I a, Vector2I b) { return (a.x * b.y - a.y * b.x); }
float crossVec2(Vector2 a, Vector2 b) { return (a.x * b.y - a.y * b.x); }
float dotVec4(Vector4 a, Vector4 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
Vector3 crossProduct(Vector3 a, Vector3 b) {
  Vector3 result;
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;
  return result;
}
int calculateDistance(int x1, int y1, int x2, int y2) {
  int dx = x2 - x1;
  int dy = y2 - y1;

  // Squaring without math.h functions
  int dxSquared = dx * dx;
  int dySquared = dy * dy;

  // Sum of squares without math.h functions
  int distanceSquared = dxSquared + dySquared;

  // Integer square root approximation
  int distance = 0;
  int bit = 1 << 30; // The second-to-top bit is set

  while (bit > distanceSquared)
    bit >>= 2; // The second-to-top bit is set to 0

  while (bit != 0) {
    if (distanceSquared >= distance + bit) {
      distanceSquared -= distance + bit;
      distance += bit << 1; // Set the current bit in distance
    }
    distance >>= 1; // Drop the last bit
    bit >>= 2;      // The second-to-top bit is set to 0
  }

  return distance; // Return the integer approximation of square root
}
int calculateDistance3D(int x1, int y1, int z1, int x2, int y2, int z2) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  int dz = z2 - z1;

  // Squaring without math.h functions
  int dxSquared = dx * dx;
  int dySquared = dy * dy;
  int dzSquared = dz * dz;

  // Sum of squares without math.h functions
  int distanceSquared = dxSquared + dySquared + dzSquared;

  // Integer square root approximation
  int distance = 0;
  int bit = 1 << 30; // The second-to-top bit is set

  while (bit > distanceSquared)
    bit >>= 2; // The second-to-top bit is set to 0

  while (bit != 0) {
    if (distanceSquared >= distance + bit) {
      distanceSquared -= distance + bit;
      distance += bit << 1; // Set the current bit in distance
    }
    distance >>= 1; // Drop the last bit
    bit >>= 2;      // The second-to-top bit is set to 0
  }

  return distance; // Return the integer approximation of square root
}
int countDigits(int number) {
  int count = 0;

  if (number < 0) {
    number = -number;
    count++;
  }

  if (number == 0)
    return 1;

  while (number != 0) {
    number /= 10;
    count++;
  }

  return count;
}
int getTextLength(char *text, int max) {
  int textLength = 0;
  for (int i = 0; i < max; i++) {
    if (text[i] != 0)
      textLength++;
  }
  return textLength;
}

void initializeTables() {
  for (int i = 0; i < TABLE_SIZE; i++) {
    float angle = (2 * M_PI * i) / TABLE_SIZE;
    cosTable[i] = myCosine(angle);
    sinTable[i] = mySine(angle);
  }
}
float fastCosine(float angle) {
  angle = fmodf_f(angle, 2 * M_PI); // Wrap the angle to [0, 2*pi]
  if (angle < 0)
    angle += 2 * M_PI; // Ensure positive angle

  int index = (int)(angle * TABLE_SIZE / (2 * M_PI)) & TABLE_MASK;
  return cosTable[index];
}
float fastSine(float angle) {
  angle = fmodf_f(angle, 2 * M_PI);
  if (angle < 0)
    angle += 2 * M_PI;

  int index = (int)(angle * TABLE_SIZE / (2 * M_PI)) & TABLE_MASK;
  return sinTable[index];
}

Vector3 CalculateRightVector(float rotationX_, float rotationY_) {
  float rightX = fastCosine(rotationY_);
  float rightY = 0;
  float rightZ = -fastSine(rotationY_);

  Vector3 rightVector;
  rightVector.x = rightX;
  rightVector.y = rightY;
  rightVector.z = rightZ;

  return rightVector;
}
Vector3 CalculateForwardVector(float rotationX_, float rotationY_) {
  float cosX = fastCosine(rotationX_);
  float sinX = fastSine(rotationX_);
  float cosY = fastCosine(rotationY_);
  float sinY = fastSine(rotationY_);

  float forwardX = sinY * cosX;
  float forwardY = sinX;
  float forwardZ = cosY * cosX;

  Vector3 forwardVector;
  forwardVector.x = forwardX;
  forwardVector.y = forwardY;
  forwardVector.z = forwardZ;

  return forwardVector;
}
