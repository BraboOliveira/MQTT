import React from 'react'
import { View, Text, FlatList, StyleSheet, Image} from 'react-native'
import Cabecalho from '../components/Cabecalho';
import {DATA} from '../components/Cabecalho/data';
import MqttLog from '../components/Mqtt/MqttLog';
export default function ListaProdutos() {

    const Item = ({ imagem, titulo }) => (
        <View style={styles.containerItem}>
          <Image style={styles.imagem} source={imagem} resizeMode="contain"  />
          <Text style={styles.texto}>{titulo}</Text>
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
    height: 168,
    margin: 5,
    borderRadius: 8,
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