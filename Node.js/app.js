const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const port = new SerialPort({
  path: 'COM3',
  baudRate: 9600
});

const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }));

let arimal1Id = '';
let arimal2Id = '';
let ActiveBattle = false;
let player1HP = '100hp';
let player2HP = '100hp';

port.on('open', () => {
  console.log('Serial port opened');
});

parser.on('data', async (line) => {
  console.log('Arduino dice: ' + line);
  const [type, name] = line.split(': ');

  switch (type.trim()) {
    case 'Arimal 1':
      arimal1Id = name.trim();
      console.log(`El personaje escogido para Arimal 1 es ${arimal1Id}`);
      break;
    case 'Arimal 2':
      arimal2Id = name.trim();
      console.log(`El personaje escogido para Arimal 2 es ${arimal2Id}`);
      break;
    default:
      console.log('Mensaje no reconocido');
  }

  if (arimal1Id && arimal2Id) {
    ActiveBattle = true;
    // await sendArimalData(arimal1Id, arimal2Id); 
    arimal1Id = '';
    arimal2Id = '';
  
    // Imprime el mensaje ACTIVE_BATTLE
    port.write('ACTIVE_BATTLE\n');
  
    // Espera 10 segundos antes de continuar
    setTimeout(() => {
      // Envía el mensaje NO_ACTIVE_BATTLE después de la espera
      port.write('NO_ACTIVE_BATTLE\n');
    }, 10000); // 10000 milisegundos equivalen a 10 segundos
  }
  
});
if (ActiveBattle){
  
}

async function sendArimalData(arimal1Id, arimal2Id) {
  const fetch = (await import('node-fetch')).default;
  const data = {
    arimalPlayer1: {
      arimalId: arimal1Id,
      hp: 100,
      attack: 10
    },
    arimalPlayer2: {
      arimalId: arimal2Id,
      hp: 100,
      attack: 10
    }
  };

  fetch('https:/localhost:3000/api/battle/arimals', {
    method: 'PUT',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(data)
  })
    .then(response => response.json())
    .then(data => {
      console.log('Success:', data);
    })
    .catch((error) => {
      console.error('Error:', error);
    });
}

port.on('error', (err) => {
  console.error('Error: ', err.message);
});
