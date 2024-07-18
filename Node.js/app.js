const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const port = new SerialPort({
  path: 'COM3',
  baudRate: 9600
});

const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }));

let arimal1 = '';
let arimal2 = '';

port.on('open', () => {
  console.log('Serial port opened');
});

parser.on('data', (line) => {
  console.log('Arduino dice: ' + line);
  const [type, name] = line.split(': ');
  
  switch (type.trim()) {
    case 'Arimal 1':
      arimal1 = name.trim();
      console.log(`El personaje escogido para Arimal 1 es ${arimal1}`);
      break;
    case 'Arimal 2':
      arimal2 = name.trim();
      console.log(`El personaje escogido para Arimal 2 es ${arimal2}`);
      break;
    default:
      console.log('Mensaje no reconocido');
  }
});

port.on('error', (err) => {
  console.error('Error: ', err.message);
});
