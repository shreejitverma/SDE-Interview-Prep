/*
 * parser_probe: a small model of how a terminal splits a byte stream into
 * text and control functions (ECMA-48 clause 5.4, xterm ctlseqs).
 * It is a teaching model, not xterm's parser: it handles the 7-bit forms only
 * (ESC [ and ESC ]), ignores DCS/APC/PM/SOS strings until ST, and prints the
 * raw parameter string instead of splitting it, so an empty parameter (the
 * ECMA-48 5.4.2 default) stays visible.
 *
 * Input: bytes on stdin.  Output: one event per line.
 *   TEXT "abc"                      a run of printable bytes
 *   C0 0x0A                         a C0 control executed (also inside a sequence)
 *   CSI params=1;31 inter= final=m  a complete control sequence
 *   OSC cmd=2 data="x" term=BEL     OSC string closed by BEL or ST
 *   ESC final=7                     two-byte escape sequence
 *   CANCEL 0x18                     CAN or SUB aborted a sequence in progress
 *   RESTART                         ESC arrived inside a sequence and began a new one
 *   INCOMPLETE state=csi            input ended inside a sequence
 */
#include <stdio.h>
#include <string.h>

#define BUF 512

enum state {
  GROUND,
  ESC_SEEN,
  ESC_INTER,
  CSI_PARAM,
  CSI_INTER,
  CSI_IGNORE,
  OSC_STRING,
  OSC_ESC,
  STR_IGNORE,
  STR_ESC
};

static char text[BUF];
static size_t text_len;
static char params[BUF];
static size_t params_len;
static char inter[BUF];
static size_t inter_len;
static char osc[BUF];
static size_t osc_len;
static enum state state = GROUND;

static void flush_text(void) {
  if (text_len > 0) {
    text[text_len] = '\0';
    printf("TEXT \"%s\"\n", text);
    text_len = 0;
  }
}

static void reset_seq(void) {
  params_len = 0;
  inter_len = 0;
  osc_len = 0;
}

static void append(char *buf, size_t *len, unsigned char c) {
  if (*len + 1 < BUF) {
    buf[(*len)++] = (char)c;
  }
}

static void emit_csi(unsigned char final) {
  flush_text();
  params[params_len] = '\0';
  inter[inter_len] = '\0';
  printf("CSI params=%s inter=%s final=%c\n", params, inter, final);
}

static void emit_osc(const char *term) {
  char *semi;
  flush_text();
  osc[osc_len] = '\0';
  semi = strchr(osc, ';');
  if (semi != NULL) {
    *semi = '\0';
    printf("OSC cmd=%s data=\"%s\" term=%s\n", osc, semi + 1, term);
  } else {
    printf("OSC cmd=%s data=\"\" term=%s\n", osc, term);
  }
}

static const char *state_name(void) {
  switch (state) {
    case ESC_SEEN:
    case ESC_INTER:
      return "esc";
    case CSI_PARAM:
    case CSI_INTER:
    case CSI_IGNORE:
      return "csi";
    case OSC_STRING:
    case OSC_ESC:
      return "osc";
    case STR_IGNORE:
    case STR_ESC:
      return "string";
    case GROUND:
    default:
      return "ground";
  }
}

/* Returns 1 when the byte was consumed by the controls common to every state. */
static int common_controls(unsigned char c) {
  if (c == 0x18 || c == 0x1A) {
    if (state != GROUND) {
      flush_text();
      printf("CANCEL 0x%02X\n", c);
    }
    state = GROUND;
    return 1;
  }
  if (c == 0x1B && state != OSC_STRING && state != STR_IGNORE) {
    if (state != GROUND && state != OSC_ESC && state != STR_ESC) {
      flush_text();
      printf("RESTART\n");
    }
    if (state == OSC_ESC || state == STR_ESC) {
      return 0;
    }
    reset_seq();
    state = ESC_SEEN;
    return 1;
  }
  if (c < 0x20 && state != GROUND && state != OSC_STRING && state != OSC_ESC &&
      state != STR_IGNORE && state != STR_ESC) {
    flush_text();
    printf("C0 0x%02X\n", c);
    return 1;
  }
  return 0;
}

static void feed(unsigned char c) {
  if (common_controls(c)) {
    return;
  }
  switch (state) {
    case GROUND:
      if (c < 0x20 || c == 0x7F) {
        flush_text();
        printf("C0 0x%02X\n", c);
      } else {
        append(text, &text_len, c);
      }
      break;
    case ESC_SEEN:
      flush_text();
      if (c == '[') {
        reset_seq();
        state = CSI_PARAM;
      } else if (c == ']') {
        reset_seq();
        state = OSC_STRING;
      } else if (c == 'P' || c == 'X' || c == '^' || c == '_') {
        state = STR_IGNORE;
      } else if (c >= 0x20 && c <= 0x2F) {
        append(inter, &inter_len, c);
        state = ESC_INTER;
      } else if (c >= 0x30 && c <= 0x7E) {
        printf("ESC final=%c\n", c);
        state = GROUND;
      } else {
        state = GROUND;
      }
      break;
    case ESC_INTER:
      if (c >= 0x30 && c <= 0x7E) {
        inter[inter_len] = '\0';
        printf("ESC inter=%s final=%c\n", inter, c);
        state = GROUND;
      } else if (c >= 0x20 && c <= 0x2F) {
        append(inter, &inter_len, c);
      } else {
        state = GROUND;
      }
      break;
    case CSI_PARAM:
      if (c >= 0x30 && c <= 0x3F) {
        append(params, &params_len, c);
      } else if (c >= 0x20 && c <= 0x2F) {
        append(inter, &inter_len, c);
        state = CSI_INTER;
      } else if (c >= 0x40 && c <= 0x7E) {
        emit_csi(c);
        state = GROUND;
      } else {
        state = CSI_IGNORE;
      }
      break;
    case CSI_INTER:
      if (c >= 0x20 && c <= 0x2F) {
        append(inter, &inter_len, c);
      } else if (c >= 0x40 && c <= 0x7E) {
        emit_csi(c);
        state = GROUND;
      } else {
        state = CSI_IGNORE;
      }
      break;
    case CSI_IGNORE:
      if (c >= 0x40 && c <= 0x7E) {
        state = GROUND;
      }
      break;
    case OSC_STRING:
      if (c == 0x07) {
        emit_osc("BEL");
        state = GROUND;
      } else if (c == 0x1B) {
        state = OSC_ESC;
      } else {
        append(osc, &osc_len, c);
      }
      break;
    case OSC_ESC:
      if (c == '\\') {
        emit_osc("ST");
        state = GROUND;
      } else {
        /* ESC inside a string that is not ST abandons the string and starts a new sequence. */
        printf("RESTART\n");
        reset_seq();
        state = ESC_SEEN;
        if (c != 0x1B) {
          feed(c);
        }
      }
      break;
    case STR_IGNORE:
      if (c == 0x1B) {
        state = STR_ESC;
      }
      break;
    case STR_ESC:
      if (c == '\\') {
        state = GROUND;
      } else {
        state = STR_IGNORE;
      }
      break;
    default:
      state = GROUND;
      break;
  }
}

int main(void) {
  int ch;
  while ((ch = getchar()) != EOF) {
    feed((unsigned char)ch);
  }
  flush_text();
  if (state != GROUND) {
    printf("INCOMPLETE state=%s\n", state_name());
  }
  return 0;
}
