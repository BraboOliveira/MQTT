import React from 'react'
import { View, Text, FlatList, StyleSheet, Image, TouchableHighlight, TouchableOpacity} from 'react-native'
import Cabecalho from '../components/Cabecalho';
import {DATA} from '../components/Cabecalho/data';
import MqttLog from '../components/Mqtt/MqttLog';

export default function ListaProdutos({ navigation }) {
    const Item = ({ imagem, titulo, onpress }) => (
      <View style={styles.containerItem}>
      <TouchableOpacity
        onPress={() => navigation.navigate(onpress)}
      >
        <View style={styles.containerItem2}>
          <Image style={styles.imagem} source={imagem} resizeMode="contain"  />
          <Text style={styles.texto}>{titulo}</Text>
        </View>
        </TouchableOpacity>
      </View>
    );
    return (
        <View>
            <FlatList
                numColumns={2}
                data={DATA}
                renderItem={({item }) => <Item {...item} />}
                keyExtractor={item => item.id}
                ListHeaderComponent={
                  <Cabecalho/>
                }
            />
          
            <MqttLog/>
        </View>
    )
}

const styles = StyleSheet.create({
  containerItem:{
    flex: 1,
    backgroundColor: '#fff',
    width: 100,
    height: 178,
    margin: 5,
    borderRadius: 8,
    justifyContent: 'center',
    alignItems: 'center',
  },
  containerItem2:{
    justifyContent: 'center',
    alignItems: 'center',
  },
  imagem:{
    margin: 5,
    marginTop: 40,
    width: '50%',
    height: '50%',
  },
  texto:{
    marginTop: 20,
    fontSize: 16,
    margin: 30,
  }
})