import net from 'node:net';

const sv = net.createServer((client) => {
  client.on('data', (data) => {
    console.log('Packet received!');
  });

  client.on('end', () => {
    console.log('Client closed!');
  });

  console.log('Client connected!');
  client.write(
    '\x01\x00\x00\x00' +
    '\x25\x00\x00\x00' +
    'D:/ps4/games/Sonic Mania/eboot.bin\0\0\0'
  );
});

sv.listen('\\\\.\\pipe\\psoff-test', () => {
  console.log('Pipe listener started!');
});
