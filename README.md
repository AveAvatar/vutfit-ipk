# IPK Projekt 1 

> @autor Tadeáš Kachyňa, <xkachy00@stud.fit.vutbr.cz>

> @datum 10/03/2022

> dokumentace hinfosvc.c 

Zadání projektu bylo vytvoření jednoduchého serveru komunikujícího prostřednictvím HTTP protokolu. 

## Spuštění projektu
Spusťtě kompilaci projektu za pomocí Makefile.
```
$ make 
```
Poté můžete server spustit za pomocí následujícího příkazu, kde [port] označuje argument portu na který se chcete připojit
```
$ ./hinfosvc [port]
```
Pro zaslání požadavků na server si otevřete druhý terminál. Příklad validních požadavků je vypsáno níže, můžete použít curl nebo wget.
```
$ (curl|wget) http://localhost:<port>/(hostame|cpu-name|load)
```
Alternativní varianta je spuštění a zároveň zaslání požadavku.
```
$ ./hinfosvc [port] & (curl|wget) http://localhost:<port>/(hostame|cpu-name|load)
```
Třetí možnost je zaslání požadavku za pomocí webového prohlížeče.

Vypnutí serveru je možné stisknutím CTRL+C v terminálu.

Pro smazání spustitelného souboru zadejte:
```
$ make clean
```

## O Projektu

Úkolem bylo vytvoření komunikujícího serveru prostřednictvím protokolu HTTP, který poskytuje různé informace o systému. Server naslouchá na zvoleném portu a podle url vrací požadované informace. Server je
naimplementovan za pomocí knihovny socketů využívajících funkcí pro tvorbu socketů socket() a blízkými funkcemi jako listen(), accept() apod. V nekonečné smyčce zpracovává server požadavky na základě tzv. "endpointů", které jsou zadány uživatelem. Poté se ve větvi "if-else" vykonají příslušné funkce a příkazy. Aktuálně podporuje server tři různě endpointy - a to na zjištění jména hosta, jména procesoru a výpočtu aktuálního zatížení procesoru. Na jiné požadavky (neznámé endpointy) odpovídá server chybou 404 Not Found, na specifické situace i chybou 400 Bad Request.

Funkce na zjištění hostname a cpuname jsou naimplementovány triviálním příkazem na extrakci požadované informace ze systému za pomocí bash příkazu, případně awk. Zajímavější je implementace výpočtu aktuálního zatížení procesoru, která byla inspirována ze stránky https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux, která nám byla v zadání doporučena.

## Příklady 
```
$ ./hinfosvc 8080 & curl http://localhost:8080/hostame
$ > ubuntu
```
```
$ ./hinfosvc 8080 & curl http://localhost:8080/cpu-name
$ > Intel(R) Core(TM) i5-8257U CPU @ 1.40GHz
```
```
$ ./hinfosvc 8080 & curl http://localhost:8080/load
$ > 23%
```
