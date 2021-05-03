#ifndef GSM_H
#define GSM_H

void send_sms(String number, String text)
{
  message.println("AT");
  delay(500);
  message.println("AT+CMGF=1");
  delay(500);
  message.println("AT+CMGS=\"+91"+number+"\"\r");
  delay(500);
  message.println(text);
  delay(500);
  message.println(char(26));
  delay(1000);
}
#endif
