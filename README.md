## Description 
give the name of your desired filepath as the first argument to the program and it will save the content of the file in a doubly linked list on demand.
with arrow keys you can move forward and backward (in this sense it's more like **less** since is has pagination).
the size of buffer is based on st_blksize of the file acquired by stat systemcall and of course you can change it.
<br/>
I beleive data structers are very well understood in C/C++ since you have a full control over pointers, it's also more prone to error for naive programmers.
## structs

## functions
### ```Buffer *read_buffer (File * f, off_t index);```
when the next requested buffer in the file is not read already, you call this function to get the next chunk.
<br/>
index should be the last unread byte (which is current index plus st_blksize of the file).
<br/>

### ```void move_left (File *);```
read the previous buffer. if you reached the beginning and the end is not available do nothing else go back to the last buffer.
<br/>
### ```void move_right (File *);```
read the next buffer. if you reached the end, start from beginning.
<br/>
### ```int getch ();```
implemeted by gemini. it add the functionality of capturing arrow keys to move around the file.

### ```File *initialize (char *);```
it calls open systemcall.
<br/>
### ```void test (File * f);```
if you choose to call this function in your code, it will act like **cat**.
<br/>
