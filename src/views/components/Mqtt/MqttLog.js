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


export default function MqttLog (props){
  const { style } = props;
  const clientID = Math.floor(Math.random() * 10000) + 1;
  const [listaMqtt,setListaMqtt] = useState([])
  const [text1, setText1] = useState('');
  const [info1,setInfo1] = useState('')
  const [text2, setText2] = useState('');
  const [info2,setInfo2] = useState('')
  const [con,setCon] = useState('')
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
    if(message.payloadString.substr(0, 7) == 'frontal1'){
      console.log(message.payloadString.substr(7, 15))
      // setText(`Frontal: ${message.payloadString.substr(7, 15)}`);
      setText1(message.payloadString.substr(7, 15));
    }
    if(message.payloadString.substr(0, 8) == 'traseiro1'){
      console.log(message.payloadString.substr(8, 15))
      // setInfo(`Traseiro: ${message.payloadString.substr(8, 15)}`);
      setInfo1(message.payloadString.substr(8, 15));
    }
    console.log(message.payloadString.substr(0, 9))
    if(message.payloadString.substr(0, 9) == 'conectado'){
      console.log(message.payloadString.substr(9, 15))
      // setInfo(`Traseiro: ${message.payloadString.substr(8, 15)}`);
      setCon(message.payloadString.substr(9, 15));
    }
    if(message.payloadString.substr(0, 7) == 'frontal2'){
      console.log(message.payloadString.substr(7, 15))
      // setText(`Frontal: ${message.payloadString.substr(7, 15)}`);
      setText2(message.payloadString.substr(7, 15));
    }
    if(message.payloadString.substr(0, 8) == 'traseiro2'){
      console.log(message.payloadString.substr(8, 15))
      // setInfo(`Traseiro: ${message.payloadString.substr(8, 15)}`);
      setInfo2(message.payloadString.substr(8, 15));
    }
    console.log(message.payloadString.substr(0, 9))
    if(message.payloadString.substr(0, 9) == 'conectado'){
      console.log(message.payloadString.substr(9, 15))
      // setInfo(`Traseiro: ${message.payloadString.substr(8, 15)}`);
      setCon(message.payloadString.substr(9, 15));
    }
    // setText(`Nova Mensagem: ${message.payloadString}`);
  };

  function Envio(message) {
    message = new Paho.MQTT.Message("Vamos");
    message.destinationName = "Distancia";
    client.send(message);
  };

  return (
    <View style={styles.container}>
      <View style={styles.bloco}>
        <Text style={{fontSize:18, marginBottom: 10}}>PÉ ESQUERDO</Text>
        <Text style={{fontSize: 18, alignContent: 'center'}}>Sensor Frontal: {text1}</Text>
        <Text style={{fontSize: 18, alignContent: 'center'}}>Sensor Traseiro: {info1}</Text>
        <Text style={{fontSize: 18, alignContent: 'center'}}>Status: {con}</Text>
      </View>
      <View style={styles.bloco}>
        <Text style={{fontSize:18, marginBottom: 10}}>PÉ DIREITO</Text>
        <Text style={{fontSize: 18, alignContent: 'center'}}>Sensor Frontal: {text2}</Text>
        <Text style={{fontSize: 18, alignContent: 'center'}}>Sensor Traseiro: {info2}</Text>
        <Text style={{fontSize: 18, alignContent: 'center'}}>Status: {con}</Text>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    width: '98%',
    height: '50%',
    alignSelf: 'center',
    backgroundColor: '#fff',
    flexDirection: "row",
    borderRadius: 5,
    paddingTop: 10,
  },
  bloco: {
    width: '50%',
    height: '50%',
    margin: 5,
  },
});