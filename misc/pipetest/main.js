import net from 'node:net';

const sv = net.createServer((client) => {
  client.on('data', (data) => {
    console.log('Packet received!');
  });

  client.on('end', () => {
    console.log('Client closed!');
  });
});

sv.listen('\\\\.\\pipe\\test', () => {
  console.log('Pipe listener started!');
});
