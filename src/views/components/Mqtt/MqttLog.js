import React, {useEffect, useState} from 'react';
import init from 'react_native_mqtt';
import {StyleSheet, Text, View, Button} from 'react-native';
import AsyncStorage from '@react-native-async-storage/async-storage';

init({
  size: 10000,
  storageBackend: AsyncStorage,
  defaultExpires: 1000 * 3600 * 24,
  enableCache: true,
  sync: {},
});

const styles = StyleSheet.create({
  container: {
    width: '100%',
    height: '100%',
  },
});

export default function MqttLog (props){
  const { style } = props;
  const clientID = Math.floor(Math.random() * 10000) + 1;
  const [text, setText] = useState('');
  const [listaMqtt,setListaMqtt] = useState([])
  const [info,setInfo] = useState('')

  const [clientInfo, setClientInfo] = useState({
    BROKER: 'broker.hivemq.com',
    PORT: '8000',
    TOPIC: 'Distancia',
    TOPIC2: 'Distancia2'
  });
  
  const client = 
    new Paho.MQTT.Client(
      clientInfo.BROKER,
      Number(clientInfo.PORT),
      `clientId-${clientID}`
    )

  
  useEffect(() => {
    try{
      client.connect({onSuccess: onConnect, useSSL: false});
      console.log('Conectado!');
    } catch (err) {
      console.log(`Erro na conexão. Error: ${err.message}.`);
    }
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    return () => client.disconnect();
  }, []);
  
  function pushText (entry) {
    setInfo(entry);
    console.log(entry);
  };

  function onConnect () {
    try{
      //se inscrever em tópico
      client.subscribe(clientInfo.TOPIC);
      client.subscribe(clientInfo.TOPIC2);
      //Envio de Mensagem
      // message = new Paho.MQTT.Message("Vamos");
      // message.destinationName = "WORLD";
      // client.send(message);
      //Termina Envio
      // pushText(`Client subscribed in Topic ${clientInfo.TOPIC}!`);
    } catch (err) {
      push('Client can not subscribed!');
      console.log(`Client can not subscribed! Error: ${err.message}.`);
    }
  };

  function onConnectionLost (responseObject) {
    if (responseObject.errorCode !== 0) {
      pushText(`connection lost: ${responseObject.errorMessage}`);
    }
  };

  function onMessageArrived (message) {
    if(message.payloadString.substr(0, 7) == 'frontal'){
      console.log(message.payloadString.substr(0, 7))
      setText(`Sensor ${message.payloadString}`);
    }
    if(message.payloadString.substr(0, 8) == 'traseiro'){
      console.log(message.payloadString.substr(0, 8))
      setInfo(`Sensor ${message.payloadString}`);
    }
    // setText(`Nova Mensagem: ${message.payloadString}`);
  };

  function Envio(message) {
    message = new Paho.MQTT.Message("Vamos");
    message.destinationName = "Distancia";
    client.send(message);
  };

  return (
    <View style={style}>
      <Text style={{fontSize:24, marginBottom: 10}}>Quantidades de toque:</Text>
      <Text style={{fontSize: 24, alignContent: 'center'}}>{text}</Text>
      <Text style={{fontSize: 24, alignContent: 'center'}}>{info}</Text>
      {/* <Button
              onPress={()=>{Envio('teste')}}
              title="Envio"
              color="#841584"
              accessibilityLabel="Learn more about this purple button"
      /> */}
    </View>
  );
}