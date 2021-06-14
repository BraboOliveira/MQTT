import React from 'react'
import { View, Text, SafeAreaView, Image, StyleSheet} from 'react-native'
import Cabecalho from './views/components/Cabecalho'
import ListaProdutos from './views/ListaProdutos'
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import SensorInercial from './views/IMU/SensorInercial';
import SensordePassos from './views/SensordePassos/SensordePassos';

export default function App() {
  const Stack = createStackNavigator();

  return (
    <NavigationContainer>
      <Stack.Navigator>
        {/* <SafeAreaView style={styles.container}> */}
          {/* <Cabecalho/> */}
          <Stack.Screen name="Home" component={ListaProdutos} />
          <Stack.Screen name="Sensor Inercial" component={SensorInercial} />
          <Stack.Screen name="Sensor de Passos" component={SensordePassos} />
        {/* </SafeAreaView> */}
      </Stack.Navigator>
    </NavigationContainer>

  )
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