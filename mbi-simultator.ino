template<typename T> void swap(T &t1, T &t2) {
  T tmp(t1);
  t1 = t2;
  t2 = tmp;
}


/* Yorkshire Surprise Major */

const char *method = "-38-14-58-16-12-38-14-78,12";  // Yorkshire
const size_t lead_length = 32;                       // Surprise
const uint8_t stage = 8;                             // major
const char *bob = "14";
const char *single = "1234";
const size_t call_at = lead_length - 1;
const char *composition = "     -  -   -      -  -   -      -  -   - ";   // 1344 Yorkshire Surprise Major


/* Grandsire Doubles */
/*
const char *method = "3,1.5.1.5.1";  // Grandsire
const size_t lead_length = 10;       //
const uint8_t stage = 5;             // Doubles
const char *bob = "3.1";
const char *single = "3.123";
const size_t call_at = lead_length - 2;
const char *composition = "  -  -  s  -  -  s";  // 180 Grandsire Doubles
*/

char *parsed_method[lead_length];
char *parsed_bob[lead_length];
char *parsed_single[lead_length];
size_t m = 0;  // index into method
size_t c = 0;  // index into composition

char bells[stage + 1];  // +1 for zero termination

bool handstroke = true;
const int gap = 250;

void print(char *bells) {
  for (int i = 0; i < stage; i++) {
    Serial.print(bells[i]);
    delay(gap);
  }
  if (!handstroke)
    delay(gap);
  handstroke = !handstroke;
  Serial.println();
}

bool is_rounds(char *input) {
  for (size_t i = 0; i < stage; i++)
    if (input[i] != (char)('1' + i))
      return false;
  return true;
}

char *strdupwithoutspaces(const char *input) {
  size_t count = 0;
  for (const char *i = input; *i; i++)
    if (*i != ' ')
      count++;
  char *output = (char *)malloc((count + 1) * sizeof(char));
  char *j = output;
  for (const char *i = input; *i; i++)
    if (*i != ' ')
      *j++ = *i;
  *j = 0;
  return output;
}

void set_rounds(char *input) {
  size_t i;
  for (i = 0; i < stage; i++)
    input[i] = '1' + i;
  input[i] = 0;
}

size_t parse(const char *notation, char *results[]) {
  size_t r = 0;
  size_t block_start = 0;
  bool within_fixed = false;
  bool mirrored = false;
  char fixed[stage + 1];  // +1 for null termination
  set_rounds(fixed);

  for (const char *n = notation; *n; n++)
    switch (*n) {
      case '.':  // end of group
        if (within_fixed) {
          results[r++] = strdupwithoutspaces(fixed);
          set_rounds(fixed);
          within_fixed = false;
        }
        break;
      case ',':  // add duplicate reversed
        mirrored = true;
        if (within_fixed) {
          results[r++] = strdupwithoutspaces(fixed);
          set_rounds(fixed);
          within_fixed = false;
        }
        if (r - block_start > 1) {
          size_t block_end = r - 2;
          int count = block_end - block_start + 1;
          for (int i = 0; i < count; i++)
            results[r++] = strdupwithoutspaces(results[block_end - i]);
        }
        block_start = r;
        break;
      case '-':  // all change
        if (within_fixed) {
          results[r++] = strdupwithoutspaces(fixed);
          set_rounds(fixed);
          within_fixed = false;
        }
        results[r++] = strdupwithoutspaces(fixed);
        break;
      case '1':  // treble
        within_fixed = true;
        fixed[0] = ' ';
        break;
      case '2':  // two
        within_fixed = true;
        fixed[1] = ' ';
        break;
      case '3':
        within_fixed = true;
        fixed[2] = ' ';
        break;
      case '4':
        within_fixed = true;
        fixed[3] = ' ';
        break;
      case '5':
        within_fixed = true;
        fixed[4] = ' ';
        break;
      case '6':
        within_fixed = true;
        fixed[5] = ' ';
        break;
      case '7':
        within_fixed = true;
        fixed[6] = ' ';
        break;
      case '8':
        within_fixed = true;
        fixed[7] = ' ';
        break;
      default:
        break;
    }
  if (within_fixed)
    results[r++] = strdupwithoutspaces(fixed);
  if (mirrored && r - block_start > 1) {
    size_t block_end = r - 2;
    int count = block_end - block_start + 1;
    for (int i = 0; i < count; i++)
      results[r++] = strdupwithoutspaces(results[block_end - i]);
  }
  return r;
}

void print_parsed_data(const char *prefix, char *data[]) {
  Serial.print(prefix);
  for (size_t i = 0; data[i]; i++) {
    Serial.print(data[i]);
    Serial.print(", ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(2400);

  set_rounds(bells);  // start at rounds

  parse(method, parsed_method);
  parse(bob, parsed_bob);
  parse(single, parsed_single);

  // print_parsed_data("method: ", parsed_method);
  // print_parsed_data("bob: ", parsed_bob);
  // print_parsed_data("single: ", parsed_single);

  // start with 5 sets of rounds
  for (int i = 0; i < 10; i++)
    print(bells);
}

void loop() {
  const char *row = parsed_method[m];
  if (m >= call_at)
    switch (composition[c]) {
      case '-':
        row = parsed_bob[m - call_at];
        break;
      case 's':
        row = parsed_single[m - call_at];
        break;
      default:
        break;
    }
  for (int i = 0; row[i]; i += 2)
    swap(bells[row[i] - '1'], bells[row[i + 1] - '1']);

  print(bells);
  m++;
  if (m == lead_length) {
    m = 0;
    c++;
  }

  if (is_rounds(bells)) {
    while (true)
      print(bells);
  }
}