# Weather Station - Back-end
Arduino back-end source code.

# What you need ?
- [ ] 1x Arduino Uno card
- [ ] 1x Arduino Ethernet shield
- [ ] 2x DHT11 sensor
- [ ] 1x LCM1 602 IIC V1
- [ ] 1x [Anémometre GRLEX003](https://www.lextronic.fr/temperature-meteo/19999-capteur-anenometre.html)
- [ ] 1x Motor
- [ ] 3x Servo
- [ ] 5x LED (2x yellow, 2x red et 1x blue)

# API Endpoints
### Core
- `/data`: Get temperature & humidity
- `/reboot`: Reboot the card
- `/auto?on={bool}`: enable / disable automatic mode
### Led colors
- `/red`
- `/green`
- `/blue`
- `/white`
- `/red-green`
- `/green-blue`
- `/blue-red`

## Prototype
![Prototype](https://raw.githubusercontent.com/lucienbl/weather-station-back-end/master/Weather_station.png)

