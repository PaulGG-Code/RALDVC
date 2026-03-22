from pwn import *

context.log_level = 'error'
p = process('./patched', stdin=PIPE, stdout=PIPE, stderr=PIPE)
out = b''
while b'Enter log message:' not in out:
    c = p.recv(timeout=1)
    if not c:
        break
    out += c
p.sendline(b'%x.%x.%x.%n')
result = p.recvall(timeout=2).decode(errors='replace')
p.close()
print('[result]', result.strip())
if 'ADMIN' in result:
    print('[-] UNEXPECTED: exploit succeeded!')
else:
    print('[+] EXPECTED: %%x/%%n printed literally, no memory corruption.')
