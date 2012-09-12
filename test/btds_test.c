#include <btds.h>

int main (int argc, char **argv)
{
  int dead, ch, cnt = 0, bits = 16;
  uint slot, line = 0, off = 0;
  unsigned char key[256];
  uint done, start;
  uint scan = 0;
  uint len = 0;
  uint map = 0;
  BtKey ptr;
  BtDb *bt;
  FILE *in;

  if( argc < 4 ) {
    printf ("Usage: %s idx_file src_file Read/Write/Scan/Delete [page_bits pool_size start_line_number]\n", argv[0]);
    exit(0);
  }

  start = (uint)time(NULL);

  if( argc > 4 )
    bits = atoi(argv[4]);

  if( argc > 5 )
    map = atoi(argv[5]);

  if( argc > 6 )
    off = atoi(argv[6]);

  bt = bt_open ((argv[1]), BT_rw, bits, map);

  if( !bt ) {
    printf("Index Open Error %s\n", argv[1]);
    exit (1);
  }

  switch(argv[3][0]| 0x20)
    {
    case 'w':
      printf("started indexing for %s at: %u\n", argv[2], start);
      if( argc > 2 && (in = fopen (argv[2], "rb")) )
        while( ch = getc(in), ch != EOF )
          if( ch == '\n' )
            {
              if( off )
                sprintf((char *)key+len, "%.9d", line + off), len += 9;

              if( bt_insertkey (bt, key, len, 0, ++line, start) )
                printf("Error %d Line: %d\n", bt->err, line), exit(0);
              len = 0;
            }
          else if( len < 245 )
            key[len++] = ch;
      printf("finished adding keys, %d \n", line);
      break;

    case 'd':
      printf("started deleting keys for %s at: %u\n", argv[2], start);
      if( argc > 2 && (in = fopen (argv[2], "rb")) )
        while( ch = getc(in), ch != EOF )
          if( ch == '\n' )
            {
              if( off )
                sprintf((char *)key+len, "%.9d", line + off), len += 9;
              line++;
              if( bt_deletekey (bt, key, len, 0) )
                printf("Error %d Line: %d\n", bt->err, line), exit(0);
              len = 0;
            }
          else if( len < 245 )
            key[len++] = ch;
      printf("finished deleting keys, %d \n", line);
      break;

    case 's':
      scan++;
      break;

    }

  dead = cnt = 0;
  len = key[0] = 0;

  printf("started reading, %d \n", line);

  if( (slot = bt_startkey (bt, key, len)) )
    slot--;
  else
    printf("Error %d in StartKey. Line: %d\n", bt->err, line), exit(0);

  while( (slot = bt_nextkey (bt, slot)) )
    if( cnt++, scan ) {
      ptr = bt_key(bt, slot);
      fwrite (ptr->key, ptr->len, 1, stdout);
      fputc ('\n', stdout);
    }

  done = (uint)time(NULL);
  printf("Done at : %u \n Time to complete: %u seconds\n", done, done - start);
  printf(" Total keys read %d \n", cnt);
}
