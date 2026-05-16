# 🚢 Torpedó (Battleship) Játék - C++ GraphicsLib

Ez a repository egy C++ nyelven, SDL2 és az egyedi `GraphicsLib` (genv) felhasználásával készült Torpedó (Battleship) játékot tartalmaz. A projekt a "Bevezetés a programozásba II." tantárgy 3. beadandó feladataként készült.

## ✨ Funkciók és Jellemzők
A játék maradéktalanul teljesíti a beadandó követelményeit:
* 🧩 **Objektumorientált felépítés:** A grafikus felület és a játéklogika szigorúan el van választva. A játékszabályokat a `JatekMester` (GameMaster) osztály felügyeli.
* 📦 **Kibővített Widget készlet:** A meglévő widget könyvtár (`Widget`, `Button`, `List`) kiegészült egy egyedi `BoardWidget`-tel a játéktér megjelenítésére, valamint egy `TextWidget`-tel, hogy minden képernyőre kerülő grafikai elem szabályosan `Widget` leszármazott legyen.
* ⚓ **Flotta felállítás:** A játék egy hajó-lehelyezési (Placement) fázissal kezdődik, ahol a játékos elhelyezheti a különböző méretű hajókból álló flottáját. A hajók forgathatóak (Jobb klikk).
* 💥 **Ütések, Süllyedések és Eredményjelző:** A játék interaktívan jelzi a találatokat, a mellélövéseket és a süllyedéseket. Az aktuális állást egy folyamatosan frissülő eredményjelző mutatja.
* 🛡️ **Bolondbiztos működés:** A program minden egér- és billentyűzet-eseményt stabilan kezel.

### 🌟 Extra pontok (Felturbózott játékmód)
A játék az extra pontszerzési lehetőségnek megfelelően tartalmaz egy listás kiválasztó widgetet, amivel a játékos a körében **különleges lövéstípusokat** alkalmazhat:
1. 🎯 **Normál lövés:** Klasszikus célzás (1 mező).
2. ✝️ **Kereszt lövés:** A kiválasztott pont alapján a teljes célzott sort és oszlopot egyszerre letarolja. Ennek az az értelme, hogy hatalmas területet fed le egyetlen lövéssel, így radarként és tömegpusztító fegyverként is funkcionál: garantáltan felfedezel vele legalább egy hajótáékot, ha van a vonalban!
3. 💣 **Szőnyegbomba:** A célpont 3x3-as környezetét egyszerre bombázza.


## 📜 Fejlesztési Mérföldkövek (Git)
A projekt a követelményeknek megfelelően, logikusan felépített verziókövetési fázisokban készült el:
1. **v0.1** - Meglévő Widget könyvtár alapjainak rögzítése.
2. **v0.2** - A Torpedó alapvető logikájának, a GameMaster-nek és a BoardWidget-nek a megalkotása.
3. **v0.3** - Az extra lövéstípusok hozzáadása kiválasztó widget segítségével.
