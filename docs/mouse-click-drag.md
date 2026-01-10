# Kliknięcie + przeciągnięcie do brzegu — dokumentacja

Krótki opis zachowania obsługi myszy (implementacja w `mouse.cx`) — jak współdziałają zdarzenia kliknięcia oraz przeciągnięcia kursora do brzegu ekranu.

---

## Główne punkty ❗️

- **Funkcja i miejsce**: logika znajduje się w funkcji `E_mouse_I_interrupt` w pliku `mouse.cx`.
- **Stany przycisku**: `E_mouse_Q_button_Z_state` — `S_clean`, `S_press`, `S_release`, `S_press_held`, `S_press_held_release`, `S_dirty`.
- **Progi ruchu**: `E_mouse_Q_button_S_click_dx`, `E_mouse_Q_button_S_click_dy` — maks. przesunięcie, przy którym akcja wciąż traktowana jest jako klik.
- **Timery**: `press_timeout`, `release_timeout`, `dirty_timeout`, `border_timeout` — kontrolują wykrywanie pojedynczych/podwójnych klików, stanu dirty i akcji przy krawędzi.

---

## Najważniejsze reguły (precyzyjnie) ✅

- Przeciągnięcie do brzegu (dotarcie kursora do `x == 0` lub `x == width-1`) ustawia *specjalny stan brzegowy*:
  - `E_mouse_Q_button_S_click_count = ~0;`
  - Startuje `border_timeout` przez `E_flow_Q_spin_time_M(&E_mouse_Q_button_S_time, E_mouse_Q_button_S_border_timeout * 1000)`.
  - Wywoływane jest `Yi_F(mouse, click, E_mouse_Q_button_S_border_timeout)` — **ustawia odliczanie** do wzbudzenia handlera (startuje timer dla callbacku). Zmienna `E_mouse_Q_button_S_time` jest ustawiana w tej samej chwili (wartość w mikrosekundach) i służy jako pomocniczy znacznik czasu; handler przed wykonaniem sprawdza ten znacznik oraz aktualny stan (np. czy nie stał się `S_dirty`) i rezygnuje z działania, jeśli wykryje zmianę stanu w międzyczasie.

- Jeśli w trakcie tego odliczania wystąpi zdarzenie kliknięcia (zmiana stanu przycisku), obsługa kliknięcia **nadpisuje** aktualny timer (np. `press_timeout`/`release_timeout`/`dirty_timeout`) i zmienia `E_mouse_Q_button_S_state` — w praktyce **resetuje**/anuluje odliczanie brzegowe.

- Przejście ruchu do stanu `S_dirty` nie jest rozpoznaniem przeciągnięcia — **jest używane jako tymczasowe zablokowanie** interpretacji zarówno kliknięć, jak i przeciągnięć na określony czas. Następuje ono **tylko** gdy:
  - aktualny stan przycisku to **`S_press`** lub **`S_release`**,
  - oraz przesunięcie przekracza wartości `E_mouse_Q_button_S_click_dx`/`click_dy`.
  W tej sytuacji wykonywane jest:
  - `E_mouse_Q_button_S_state = E_mouse_Q_button_Z_state_S_dirty;`
  - `E_flow_Q_spin_time_M(&E_mouse_Q_button_S_time, E_mouse_Q_button_S_dirty_timeout * 1000);`
  - wywoływane jest `Yi_L(mouse, click)` — **anuluje/wyłącza odliczanie** handlera (zapobiega wykonaniu wcześniej zaplanowanego callbacku).

**Mechanika handlera i znaczenie `E_mouse_Q_button_S_time`**:
- `Yi_F(mouse, click, t)` ustawia odliczanie (schedule) — po czasie `t` handler może zostać wzbudzony.
- `Yi_L(mouse, click)` anuluje/wyłącza to odliczanie (cancel), tak aby wcześniej zaplanowany handler nie był wykonany.
- `E_flow_Q_spin_time_M(&E_mouse_Q_button_S_time, t_us)` ustawia pomocniczy znacznik czasu (`t_us` w mikrosekundach). Handler, po wzbudzeniu, porównuje oczekiwany znacznik z aktualną wartością `E_mouse_Q_button_S_time` i **sprawdza też**, czy bieżący stan nie jest `S_dirty`. Jeśli wartości różnią się lub stan zmienił się, handler rezygnuje z wykonania akcji — to zabezpiecza przed działaniem na nieaktualnym stanie spowodowanym przez intervenujące przerwania.

- Jeśli stan to **`S_press_held`** (przytrzymanie), sam ruch kursora **nie** przełączy stanu na `S_dirty` — przytrzymanie ma pierwszeństwo.

> Wniosek: przeciągnięcie do brzegu nie zostanie ostatecznie zrealizowane, jeśli w międzyczasie wystąpi kliknięcie — kliknięcie resetuje odliczanie i nadpisuje stan.

---

## Scenariusze testowe (propozycje) 🧪

1. **Szybki klik**: naciśnij i zwolnij bez przekroczenia DX/DY → zlicza jako klik (single/double zgodnie z timeoutami).
2. **Przeciągnięcie** (blokada): naciśnij (stan `S_press`) → przesuń > DX/DY → ustawiany jest stan `S_dirty` (tymczasowa blokada interpretacji) → wywoływane jest `Yi_L(mouse, click)` (anulowanie wcześniej zaplanowanego handlera), co zapobiega wykonaniu odroczonego callbacku.
3. **Przeciągnięcie do brzegu i kliknięcie w trakcie**: przeciągnij kursorem do krawędzi (uruchamia `border_timeout`), w trakcie odliczania wykonaj kliknięcie → `border_timeout` zostaje nadpisany/anulowany.
4. **Przytrzymanie + ruch**: naciśnij i przytrzymaj (`S_press_held`) → przesuń > DX/DY → stan **nie** przechodzi do `S_dirty`.

---

## Dalsze działania / rozszerzenia 📌

### Obsługa zdarzeń rolki (wheel events) 🎡

- **Cel**: dodać obsługę zdarzeń rolki myszy, przy czym **zdarzenia wheel nie powinny ustawiać stanu `S_dirty`** ani w inny sposób blokować/zmieniać licznika kliknięć lub stanów `S_press`/`S_press_held`.

- **Semantyka integracji z istniejącym handlerem**:
  - Preferowane jest, aby **ten sam handler**, który obsługuje kliknięcia i brzegowe zdarzenia (wzbudzany przez `Yi_F`/`Yi_L`) także obsługiwał wydarzenia wheel. Ten handler: blokuje przerwania (chroni przed wyścigami stanów) i *synchroncznie* wywołuje procedury GUI (`gui.cx`).
  - Zdarzenie wheel powinno być **schedulowane** przez `Yi_F(mouse, wheel, t)` (gdzie `t` może być 0 lub niewielkim opóźnieniem). Nie powinno bezpośrednio ustawiać `S_dirty` ani ingerować w `E_mouse_Q_button_S_click_count`.
  - W tej implementacji delta przewinięcia jest dostępna w zmiennej `v` przy `E_mouse_S_interrupt_state == 3` i powinna być zapisana jako `(S8)v` do pomocniczej zmiennej (u nas `E_mouse_Q_pending_wheel`), a następnie wzbudzony ten sam handler co dla kliknięć, np. przez `Yi_F(mouse, click, 0)`.
  - Jeśli podczas oczekiwania na handler wystąpi zdarzenie kliknięcia, kliknięcie ma priorytet i powinno zadziałać tak, by **anulować** zaplanowany handler wheel (np. wywołując `Yi_L(mouse, wheel)` lub polegając na porównaniu znacznika `E_mouse_Q_button_S_time` w handlerze).

- **Rola `E_mouse_Q_button_S_time`**:
  - Przy schedulowaniu wheel warto ustawić `E_mouse_Q_button_S_time` (tak jak dla kliknięć) — handler po wzbudzeniu porówna wartość i sprawdzi, czy nie nastąpiła zmiana stanu (np. `S_dirty`) między czasem schedulowania a momentem wykonywania; w razie rozbieżności handler rezygnuje z działania.

- **Zasady bezpieczeństwa / właściwe zachowanie**:
  - Wheel nie powinien resetować `border_timeout` ani innych timeoutów kliknięć, chyba że to intencjonalne.
  - Wheel nie powinien modyfikować `E_mouse_Q_button_S_click_count` ani powodować przejść stanu przycisku.
  - Handler musi sprawdzać `E_mouse_Q_button_S_time` i czy stan nie jest `S_dirty` przed wykonaniem akcji GUI.

- **Sugestie implementacyjne**:
  - Dodaj placeholder w `mouse.cx`: `Yi_F(mouse, wheel, 0)` w miejscu obsługi zdarzeń urządzenia (gdzie dekodowane są sygnały rolki). Alternatywnie wystawić funkcję `E_mouse_I_wheel(N8 delta)` która będzie schedulować handler.
  - Rozszerzyć istniejący handler click/edge tak, aby przy wzbudzeniu rozpoznawał typ zdarzenia (`click` vs `wheel`) i wywoływał odpowiedni GUI-call (np. `E_gui_Q_pointer_I_wheel(...)` lub rozszerzyć `E_gui_Q_pointer_I_click` o parametr typu).

- **Scenariusze testowe (proponowane)**:
  1. Wheel w trybie idle: scheduluj wheel i sprawdź, że GUI otrzymuje event.
  2. Wheel podczas `S_press_held`: scheduluj wheel → handler wykonuje się (o ile stan nie zmienił się), ale nie zmienia stanu przycisku i nie ustawia `S_dirty`.
  3. Wheel schedulowany, potem kliknięcie: kliknięcie powinno anulować wheel (handler nie wykonuje akcji). Test: wywołaj `Yi_F(mouse, wheel, 0)` a w krótkim czasie symuluj klik; oczekuj, że GUI nie otrzyma wheel.
  4. Wheel przy aktywnym `border_timeout`: wheel nie powinien resetować `border_timeout` (chyba że wymagane) — test sprawdzający, że `border_timeout` nadal wygasa jak wcześniej.

---


Plik przygotowany do dalszej edycji — jeśli chcesz, mogę teraz dodać sekcję placeholder dla obsługi rolki oraz krótką listę testów jednostkowych/integracyjnych do wykonania przy implementacji wheel. 

(Referencje w kodzie: `E_mouse_I_interrupt`, `E_mouse_Q_button_S_state`, `E_mouse_Q_button_S_click_dx/dy`, `E_mouse_Q_button_S_border_timeout`, `E_flow_Q_spin_time_M`, `Yi_F`/`Yi_L`.)
