int tempoDescida = 2000;
int tempoSubida = 2000;

int valor = 0;
int Botao = 2;
int levantar = 3;
int descer = 4;
bool estado = false;


void setup() {
  Serial.begin(9600);
  pinMode(Botao,INPUT);
  pinMode(levantar,OUTPUT);
  pinMode(descer,OUTPUT);
  digitalWrite(levantar,HIGH);
  digitalWrite(descer,HIGH);
}

void loop() {
 valor = digitalRead(Botao);
 if(valor == 1 && estado == false){
  Serial.println("Levantou a Antena");
  digitalWrite(levantar,LOW);
  digitalWrite(descer,HIGH);
  delay(tempoSubida);
  digitalWrite(levantar,HIGH);
  digitalWrite(descer,HIGH);
  estado = true;
  } 
  else if(valor == 0 && estado == true){
    Serial.println("Baixou a Antena");
    digitalWrite(levantar,HIGH);
    digitalWrite(descer,LOW);
    delay(tempoDescida);
    digitalWrite(levantar,HIGH);
    digitalWrite(descer,HIGH);
    estado = false;
    }
    delay(500);
}
