from pwn import *
import os

context.log_level = 'error'
p = process('./patched', stdin=PIPE, stdout=PIPE, stderr=PIPE)
out_before = b''
while b'Enter note' not in out_before:
    c = p.recv(timeout=1)
    if not c:
        break
    out_before += c
p.sendline(b'A' * 200)
out = p.recvall(timeout=2).decode(errors='replace')
p.close()
print('[result]', out.strip())
if 'SECRET' in out:
    print('[-] UNEXPECTED: exploit succeeded on patched binary!')
else:
    print('[+] EXPECTED: patched binary correctly rejected oversized input.')
