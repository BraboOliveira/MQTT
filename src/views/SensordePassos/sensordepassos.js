import React from 'react'
import { View, Text, StyleSheet} from 'react-native'

export default function SensordePassos() {
    return (
        <View style={styles.container}>
            <Text style={styles.Text}>Este aplicativo encontra-se em desenvolvimento, entre em contato com o desennvolvedor para mais detalhes!</Text>
        </View>
    )
}

const styles = StyleSheet.create({
    container: {
      width: '90%',
      height: '50%',
      alignSelf: 'center',
      alignItems: 'center',
      backgroundColor: '#fff',
      flexDirection: "row",
      borderRadius: 5,
      paddingTop: 10,
    },
    Text: {
      width: '80%',
      height: '50%',
      margin: 20,
    },
  });