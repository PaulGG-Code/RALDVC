from pwn import *
import os

context.log_level = 'error'
p = process('./patched', stdin=PIPE, stdout=PIPE, stderr=PIPE)
p.sendline(b'A' * 200)
p.sendline(b'wrongpassword')
out = p.recvall(timeout=2).decode(errors='replace')
p.close()
print('[result]', out.strip())
if 'ACCESS GRANTED' in out:
    print('[-] UNEXPECTED: exploit succeeded on patched binary!')
else:
    print('[+] EXPECTED: patched binary correctly rejected the overflow.')
