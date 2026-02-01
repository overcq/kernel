# Formatowanie składni kodu OUX/C+

Dokument ten opisuje skrócone zasady formatowania kodu obowiązujące w projekcie, na podstawie publikacji [Formatowanie składni kodu OUX/C+](http://overcq.ct8.pl/formatowanie-skladni-kodu-ouxc.html) oraz analizy kodu źródłowego jądra.

## 1. Wcięcia (Indentation)
* **Standardowe wcięcie**: 4 znaki odstępu (spacje).
* **Wcięcie w blokach danych/parametrów**: 2 znaki odstępu. Dotyczy to definicji `enum`, `struct`, przypisywania wartości, oraz list parametrów procedur.

## 2. Deklaracje i definicje procedur
* **Parametry**: Każdy parametr umieszczany jest w osobnej linii.
* **Separatory**: Przecinki oddzielające parametry znajdują się na **początku** linii.
* **Nawiasy**:
  * Nawias otwierający `(` znajduje się w linii z nazwą procedury.
  * Nawias zamykający `)` i klamra otwierająca `{` są złączone jako `){` i umieszczone w nowej linii (po ostatnim parametrze).

Przykład:
```c
N
E_ssd_cache_I_read( I disc_id
, N pba
, N count
, P *data
, N *r
){  *r = 0;
    // Ciało funkcji
}
```

## 3. Bloki wartości i inicjalizacja
* Wartości w blokach (np. inicjalizacja struktur, tablic) umieszczane są w osobnych liniach.
* Znaki rozdzielające (przecinki, średniki) umieszczane są na **początku** linii.

Przykład:
```c
struct sigaction sa =
{ .sa_handler = SIG_DFL
, .sa_flags = 0
};
```

## 4. Instrukcje sterujące (if, while, for)
* **Warunki wieloliniowe**:
  * Kolejne warunki umieszczane są w nowych liniach.
  * Operatory logiczne `&&`, `||` rozpoczynają linię.
  * Grupowanie warunków nawiasami powoduje wcięcie o 2 spacje dla każdej zagnieżdżonej grupy.
* **Nawiasy klamrowe**:
  * Klamra otwierająca `{` znajduje się w pierwszej linii bloku (chyba że to definicja funkcji, patrz pkt 2).
  * `else` jest sklejane z klamrą zamykającą: `}else`.

Przykład warunku:
```c
if( pba >= cache_start
&& pba < cache_end
) // Początek żądanego sektora jest wewnątrz bloku ‘cache’.
    break;

if( cache_i == min )
{   // Blok if
}else
{   // Blok else
}
```

## 5. Odstępy wewnątrz nawiasów
* Ogólna zasada: Umieszcza się jedną spację po nawiasie otwierającym `(` i przed zamykającym `)`.
* **Wyjątek**: Odstępy pomija się, jeśli wyrażenie wewnątrz składa się wyłącznie z:
  * Małych liter (np. `if(r)`)
  * Dużych liter
  * Cyfr (ew. z prefiksem)

Przykłady:
```c
if( E_flow_I_lock_r( &disc->sata_ahci.ssd_cache_lock, r )) // Spacja po if(, brak spacji przy r )) - zależy od złożoności
// W kodzie źródłowym widać:
if( !disc->sata_ahci.ssd_cache ) // Spacje
if( cache_i == min ) // Spacje
while( cache_i != cache_i_last ) // Spacje
```

## 6. Etykiety (Labels)
* Etykiety (np. `End:`, `End_1:`) są umieszczane bez wcięcia, często w tej samej linii co instrukcja (np. `End:if( *r )`).
