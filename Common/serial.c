/*
 * Módulo de serialización extraído de la guía Beej.
 * http://beej.us/guide/bgnet/examples/pack2.c
 */

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"
#include "structures.h"
#include "serial.h"

// macros for packing floats and doubles:
#define pack754_16(f) (pack754((f), 16, 5))
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_16(i) (unpack754((i), 16, 5))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

/*
 ** pack754() -- pack a floating point number into IEEE-754 format
 */
unsigned long long int pack754(long double f, unsigned bits, unsigned expbits)
{
	long double fnorm;
	int shift;
	long long sign, exp, significand;
	unsigned significandbits = bits - expbits - 1; // -1 for sign bit

	if (f == 0.0) return 0; // get this special case out of the way

	// check sign and begin normalization
	if (f < 0) { sign = 1; fnorm = -f; }
	else { sign = 0; fnorm = f; }

	// get the normalized form of f and track the exponent
	shift = 0;
	while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
	while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
	fnorm = fnorm - 1.0;

	// calculate the binary form (non-float) of the significand data
	significand = fnorm * ((1LL<<significandbits) + 0.5f);

	// get the biased exponent
	exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

	// return the final answer
	return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

/*
 ** unpack754() -- unpack a floating point number from IEEE-754 format
 */
long double unpack754(unsigned long long int i, unsigned bits, unsigned expbits)
{
	long double result;
	long long shift;
	unsigned bias;
	unsigned significandbits = bits - expbits - 1; // -1 for sign bit

	if (i == 0) return 0.0;

	// pull the significand
	result = (i&((1LL<<significandbits)-1)); // mask
	result /= (1LL<<significandbits); // convert back to float
	result += 1.0f; // add the one back on

	// deal with the exponent
	bias = (1<<(expbits-1)) - 1;
	shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
	while(shift > 0) { result *= 2.0; shift--; }
	while(shift < 0) { result /= 2.0; shift++; }

	// sign it
	result *= (i>>(bits-1))&1? -1.0: 1.0;

	return result;
}

/*
 ** packi16() -- store a 16-bit int into a char buffer (like htons())
 */
void packi16(unsigned char *buf, unsigned int i)
{
	*buf++ = i>>8; *buf++ = i;
}

/*
 ** packi32() -- store a 32-bit int into a char buffer (like htonl())
 */
void packi32(unsigned char *buf, unsigned long int i)
{
	*buf++ = i>>24; *buf++ = i>>16;
	*buf++ = i>>8;  *buf++ = i;
}

/*
 ** packi64() -- store a 64-bit int into a char buffer (like htonl())
 */
void packi64(unsigned char *buf, unsigned long long int i)
{
	*buf++ = i>>56; *buf++ = i>>48;
	*buf++ = i>>40; *buf++ = i>>32;
	*buf++ = i>>24; *buf++ = i>>16;
	*buf++ = i>>8;  *buf++ = i;
}

/*
 ** unpacki16() -- unpack a 16-bit int from a char buffer (like ntohs())
 */
int unpacki16(unsigned char *buf)
{
	unsigned int i2 = ((unsigned int)buf[0]<<8) | buf[1];
	int i;

	// change unsigned numbers to signed
	if (i2 <= 0x7fffu) { i = i2; }
	else { i = -1 - (unsigned int)(0xffffu - i2); }

	return i;
}

/*
 ** unpacku16() -- unpack a 16-bit unsigned from a char buffer (like ntohs())
 */
unsigned int unpacku16(unsigned char *buf)
{
	return ((unsigned int)buf[0]<<8) | buf[1];
}

/*
 ** unpacki32() -- unpack a 32-bit int from a char buffer (like ntohl())
 */
long int unpacki32(unsigned char *buf)
{
	unsigned long int i2 = ((unsigned long int)buf[0]<<24) |
			((unsigned long int)buf[1]<<16) |
			((unsigned long int)buf[2]<<8)  |
			buf[3];
	long int i;

	// change unsigned numbers to signed
	if (i2 <= 0x7fffffffu) { i = i2; }
	else { i = -1 - (long int)(0xffffffffu - i2); }

	return i;
}

/*
 ** unpacku32() -- unpack a 32-bit unsigned from a char buffer (like ntohl())
 */
unsigned long int unpacku32(unsigned char *buf)
{
	return ((unsigned long int)buf[0]<<24) |
			((unsigned long int)buf[1]<<16) |
			((unsigned long int)buf[2]<<8)  |
			buf[3];
}

/*
 ** unpacki64() -- unpack a 64-bit int from a char buffer (like ntohl())
 */
long long int unpacki64(unsigned char *buf)
{
	unsigned long long int i2 = ((unsigned long long int)buf[0]<<56) |
			((unsigned long long int)buf[1]<<48) |
			((unsigned long long int)buf[2]<<40) |
			((unsigned long long int)buf[3]<<32) |
			((unsigned long long int)buf[4]<<24) |
			((unsigned long long int)buf[5]<<16) |
			((unsigned long long int)buf[6]<<8)  |
			buf[7];
	long long int i;

	// change unsigned numbers to signed
	if (i2 <= 0x7fffffffffffffffu) { i = i2; }
	else { i = -1 -(long long int)(0xffffffffffffffffu - i2); }

	return i;
}

/*
 ** unpacku64() -- unpack a 64-bit unsigned from a char buffer (like ntohl())
 */
unsigned long long int unpacku64(unsigned char *buf)
{
	return ((unsigned long long int)buf[0]<<56) |
			((unsigned long long int)buf[1]<<48) |
			((unsigned long long int)buf[2]<<40) |
			((unsigned long long int)buf[3]<<32) |
			((unsigned long long int)buf[4]<<24) |
			((unsigned long long int)buf[5]<<16) |
			((unsigned long long int)buf[6]<<8)  |
			buf[7];
}

/*
 ** pack() -- store data dictated by the format string in the buffer
 **
 **   bits |signed   unsigned   float   string
 **   -----+----------------------------------
 **      8 |   c        C
 **     16 |   h        H         f
 **     32 |   l        L         d
 **     64 |   q        Q         g
 **      - |                               s
 **
 **  (16-bit unsigned length is automatically prepended to strings)
 */

size_t serial_pack(unsigned char *buf, char *format, ...)
{
	va_list ap;

	signed char c;              // 8-bit
	unsigned char C;

	int h;                      // 16-bit
	unsigned int H;

	long int l;                 // 32-bit
	unsigned long int L;

	long long int q;            // 64-bit
	unsigned long long int Q;

	float f;                    // floats
	double d;
	long double g;
	unsigned long long int fhold;

	char *s;                    // strings
	unsigned int len;

	size_t size = 0;

	va_start(ap, format);

	for(; *format != '\0'; format++) {
		switch(*format) {
		case 'c': // 8-bit
			size += 1;
			c = (signed char)va_arg(ap, int); // promoted
			*buf++ = c;
			break;

		case 'C': // 8-bit unsigned
			size += 1;
			C = (unsigned char)va_arg(ap, unsigned int); // promoted
			*buf++ = C;
			break;

		case 'h': // 16-bit
			size += 2;
			h = va_arg(ap, int);
			packi16(buf, h);
			buf += 2;
			break;

		case 'H': // 16-bit unsigned
			size += 2;
			H = va_arg(ap, unsigned int);
			packi16(buf, H);
			buf += 2;
			break;

		case 'l': // 32-bit
			size += 4;
			l = va_arg(ap, long int);
			packi32(buf, l);
			buf += 4;
			break;

		case 'L': // 32-bit unsigned
			size += 4;
			L = va_arg(ap, unsigned long int);
			packi32(buf, L);
			buf += 4;
			break;

		case 'q': // 64-bit
			size += 8;
			q = va_arg(ap, long long int);
			packi64(buf, q);
			buf += 8;
			break;

		case 'Q': // 64-bit unsigned
			size += 8;
			Q = va_arg(ap, unsigned long long int);
			packi64(buf, Q);
			buf += 8;
			break;

		case 'f': // float-16
			size += 2;
			f = (float)va_arg(ap, double); // promoted
			fhold = pack754_16(f); // convert to IEEE 754
			packi16(buf, fhold);
			buf += 2;
			break;

		case 'd': // float-32
			size += 4;
			d = va_arg(ap, double);
			fhold = pack754_32(d); // convert to IEEE 754
			packi32(buf, fhold);
			buf += 4;
			break;

		case 'g': // float-64
			size += 8;
			g = va_arg(ap, long double);
			fhold = pack754_64(g); // convert to IEEE 754
			packi64(buf, fhold);
			buf += 8;
			break;

		case 's': // string
			s = va_arg(ap, char*);
			len = strlen(s);
			size += len + 2;
			packi16(buf, len);
			buf += 2;
			memcpy(buf, s, len);
			buf += len;
			break;
		}
	}

	va_end(ap);

	log_inform("Packed %ld bytes", size);
	return size;
}

/*
 ** unpack() -- unpack data dictated by the format string into the buffer
 **
 **   bits |signed   unsigned   float   string
 **   -----+----------------------------------
 **      8 |   c        C
 **     16 |   h        H         f
 **     32 |   l        L         d
 **     64 |   q        Q         g
 **      - |                               s
 **
 **  (string is extracted based on its stored length, but 's' can be
 **  prepended with a max length)
 */
void serial_unpack(unsigned char *buf, char *format, ...)
{
	va_list ap;

	signed char *c;              // 8-bit
	unsigned char *C;

	int *h;                      // 16-bit
	unsigned int *H;

	long int *l;                 // 32-bit
	unsigned long int *L;

	long long int *q;            // 64-bit
	unsigned long long int *Q;

	float *f;                    // floats
	double *d;
	long double *g;
	unsigned long long int fhold;

	char *s;
	unsigned int len, maxstrlen=0, count;

	va_start(ap, format);

	for(; *format != '\0'; format++) {
		switch(*format) {
		case 'c': // 8-bit
			c = va_arg(ap, signed char*);
			if (*buf <= 0x7f) { *c = *buf;} // re-sign
			else { *c = -1 - (unsigned char)(0xffu - *buf); }
			buf++;
			break;

		case 'C': // 8-bit unsigned
			C = va_arg(ap, unsigned char*);
			*C = *buf++;
			break;

		case 'h': // 16-bit
			h = va_arg(ap, int*);
			*h = unpacki16(buf);
			buf += 2;
			break;

		case 'H': // 16-bit unsigned
			H = va_arg(ap, unsigned int*);
			*H = unpacku16(buf);
			buf += 2;
			break;

		case 'l': // 32-bit
			l = va_arg(ap, long int*);
			*l = unpacki32(buf);
			buf += 4;
			break;

		case 'L': // 32-bit unsigned
			L = va_arg(ap, unsigned long int*);
			*L = unpacku32(buf);
			buf += 4;
			break;

		case 'q': // 64-bit
			q = va_arg(ap, long long int*);
			*q = unpacki64(buf);
			buf += 8;
			break;

		case 'Q': // 64-bit unsigned
			Q = va_arg(ap, unsigned long long int*);
			*Q = unpacku64(buf);
			buf += 8;
			break;

		case 'f': // float
			f = va_arg(ap, float*);
			fhold = unpacku16(buf);
			*f = unpack754_16(fhold);
			buf += 2;
			break;

		case 'd': // float-32
			d = va_arg(ap, double*);
			fhold = unpacku32(buf);
			*d = unpack754_32(fhold);
			buf += 4;
			break;

		case 'g': // float-64
			g = va_arg(ap, long double*);
			fhold = unpacku64(buf);
			*g = unpack754_64(fhold);
			buf += 8;
			break;

		case 's': // string
			s = va_arg(ap, char*);
			len = unpacku16(buf);
			buf += 2;
			if (maxstrlen > 0 && len > maxstrlen) count = maxstrlen - 1;
			else count = len;
			memcpy(s, buf, count);
			s[count] = '\0';
			buf += len;
			break;

		default:
			if (isdigit(*format)) { // track max str len
				maxstrlen = maxstrlen * 10 + (*format-'0');
			}
		}

		if (!isdigit(*format)) maxstrlen = 0;
	}

	va_end(ap);
}

size_t serial_pack_pcb (t_pcb* pcb, unsigned char* buff){
	int tam = 0;

	tam += serial_pack(buff+tam, "h", pcb->idProcess);
	tam += serial_pack(buff+tam, "h", pcb->PC);
	tam += serial_pack(buff+tam, "h", pcb->quantum);
	tam += serial_pack(buff+tam, "h", pcb->status);
	tam += serial_pack(buff+tam, "h", pcb->pagesCode);
	tam += serial_pack(buff+tam, "h", pcb->instructions);
	tam += serial_pack(buff+tam, "h", pcb->tags);
	tam += serial_pack(buff+tam, "h", pcb->stackPointer);
	tam += serial_pack(buff+tam, "h", pcb->exitCode);
	for (int i = 0; i < pcb->instructions; i++){
		tam += serial_pack(buff+tam, "h", (pcb->indexCode+i)->start);
		tam += serial_pack(buff+tam, "h", (pcb->indexCode+i)->offset);
	}
	for (int i = 0; i < pcb->tags; i++){
		tam += serial_pack(buff+tam, "s", (pcb->indexTag+i)->name);
		tam += serial_pack(buff+tam, "h", (pcb->indexTag+i)->PC);
	}

	tam += serial_pack_stack (pcb->stack, buff+tam);

	return tam;
}

void serial_unpack_pcb (t_pcb* pcb, unsigned char* buff){
	int tam = 0;

	serial_unpack(buff+tam, "h", &pcb->idProcess);
	tam += 2;
	serial_unpack(buff+tam, "h", &pcb->PC);
	tam += 2;
	serial_unpack(buff+tam, "h", &pcb->quantum);
	tam += 2;
	serial_unpack(buff+tam, "h", &pcb->status);
	tam += 2;
	serial_unpack(buff+tam, "h", &pcb->pagesCode);
	tam += 2;
	serial_unpack(buff+tam, "h", &pcb->instructions);
	tam += 2;
	serial_unpack(buff+tam, "h", &pcb->tags);
	tam += 2;
	serial_unpack(buff+tam, "h", &pcb->stackPointer);
	tam += 2;
	serial_unpack(buff+tam, "h", &pcb->exitCode);
	tam += 2;

	pcb->indexCode = alloc(pcb->instructions * sizeof(t_intructions));
	for (int i = 0; i < pcb->instructions; i++){
		serial_unpack(buff+tam, "h", &(pcb->indexCode+i)->start);
		tam += 2;
		serial_unpack(buff+tam, "h", &(pcb->indexCode+i)->offset);
		tam += 2;
	}
	pcb->indexTag = alloc(pcb->tags * sizeof(t_programTag));
	char tag[20];
	for (int i = 0; i < pcb->tags; i++){
		serial_unpack(buff+tam, "20s", &tag);
		tam += strlen (tag) + 2;
		pcb->indexTag[i].name = string_duplicate(tag);
		strcpy((pcb->indexTag+i)->name, tag);
		serial_unpack(buff+tam, "h", &(pcb->indexTag+i)->PC);
		tam += 2;

	}
	pcb->stack = list_create();
	serial_unpack_stack (pcb->stack, buff+tam);

}

size_t serial_pack_stack (t_list* stack, unsigned char* buff){
	int tam = 0;

	tam += serial_pack (buff+tam, "h", stack->elements_count);

	for (int i = 0; i < stack->elements_count; i++){
		t_stack* aux = alloc(sizeof(t_stack));
		aux = list_get(stack, i);
		tam += serial_pack (buff+tam, "h", aux->retPos);
		tam += serial_pack_vars (aux->args, buff+tam);
		tam += serial_pack_vars (aux->vars, buff+tam);
		tam += serial_pack (buff+tam, "hhh", aux->retVar.page, aux->retVar.offset, aux->retVar.size);
	}
	return tam;
}

size_t serial_pack_vars (t_list* lista, unsigned char* buff){
	int tam = 0;

	tam += serial_pack (buff+tam, "h", lista->elements_count);

	for (int i = 0; i < lista->elements_count; i++){
		t_var* aux = alloc(sizeof(t_var));
		aux = list_get(lista, i);
		tam += serial_pack (buff+tam, "chhh", aux->id, aux->mempos.page, aux->mempos.offset ,aux->mempos.size);
	}

	return tam;
}

void serial_unpack_stack (t_list* stack, unsigned char* buff){
	int tam = 0;
	int cant_elements;
	serial_unpack (buff+tam, "h", &cant_elements);
	tam += 2;

	for (int i = 0; i < cant_elements; i++){
		t_stack* aux = alloc(sizeof(t_stack));
		aux->args = list_create ();
		aux->vars = list_create ();

		serial_unpack (buff+tam, "h", &aux->retPos);
		tam += 2;
		tam += serial_unpack_vars (aux->args, buff+tam);
		tam += serial_unpack_vars (aux->vars, buff+tam);
		serial_unpack (buff+tam, "hhh", &aux->retVar.page, &aux->retVar.offset, &aux->retVar.size);
		tam += 6;
		list_add (stack, aux);
	}

}

size_t serial_unpack_vars (t_list* lista, unsigned char* buff){
	int tam = 0;
	int cant_elements;
	serial_unpack (buff+tam, "h", &cant_elements);
	tam += 2;

	for (int i = 0; i < cant_elements; i++){
		t_var* aux = alloc(sizeof(t_var));
		serial_unpack (buff+tam, "chhh", &aux->id, &aux->mempos.page, &aux->mempos.offset, &aux->mempos.size);
		tam += 7;
		list_add (lista, aux);
	}

	return tam;
}

size_t serial_pack_memreq(t_memreq* memreq, unsigned char* buff){
	int tam = 0;

	tam += serial_pack(buff+tam, "h", memreq->idProcess);
	tam += serial_pack(buff+tam, "h", memreq->pages);

	return tam;
}

size_t serial_unpack_memreq(t_memreq* memreq, unsigned char* buff){
	int tam = 0;
//	int cant_elements;

	serial_unpack(buff+tam, "h", &memreq->idProcess);
	tam += 2;
	serial_unpack(buff+tam, "h", &memreq->pages);
	tam += 2;

	return tam;
}
