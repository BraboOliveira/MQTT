import React from 'react';
import { View, Text , StyleSheet, Image} from 'react-native'

export default function Cabecalho() {
    return (
    <>
    <View style={styles.cabecalho}>
        <Text style={styles.text}>Sensor Passos</Text>
          <View style={styles.containerImage}>
            <Image source={
              require('../../../assets/images/logo.jpeg')} 
              style={styles.image}/>
          </View>
      </View>
      <View style={styles.containerDescricao}>
        <View style={styles.separador}/>
        <View style={styles.containerTexto}>
            <Text style={styles.textoDescricao}>Sensores</Text>
        </View>
      </View>
    </>
    );
};
const styles = StyleSheet.create({
    container:{
      flex: 1,
      backgroundColor: '#F4F0F4',
    },
    image: {
      height:60,
      width: 60,
    },
    text:{
      fontSize: 35,
      fontFamily: 'open-sans-extrabold',
    },
    cabecalho:{
      flexDirection: 'row',
      justifyContent: 'space-between',
      alignItems: 'center',
      padding: 20,
    },
    containerImage: {
      backgroundColor: '#fff',
      padding: 20,
      borderRadius: 50,
    },
    containerDescricao:{
      paddingHorizontal: 20,
    },
    separador:{
      borderWidth: 0.5,
      borderColor: '#A1A5AA',
      color: '#a1a5aa'
    },
    containerTexto: {
      flexDirection: 'row',
      justifyContent: 'center',
      marginTop: -46,
    },
    textoDescricao:{
      fontSize: 20,
      padding: 34,
      backgroundColor:'#F4F0F4',
      color: '#a1a5aa'
    }
  
  })
