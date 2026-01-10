# Diagramy stanów — myszy (klik, dirty, border, wheel)

Krótki zestaw diagramów ASCII opisujących maszyny stanów przycisków oraz cykl życia handlera (w tym obsługa rolki) w implementacji `mouse.cx`.

---

## Legenda
- Stany: `S_clean`, `S_press`, `S_release`, `S_press_held`, `S_press_held_release`, `S_dirty`
- Akcje:
  - `Yi_F(mouse, click, t)` — schedule handler (ustaw timer/wzbudź)
  - `Yi_L(mouse, click)` — cancel scheduled handler
  - `E_flow_Q_spin_time_M(&time, t_us)` — ustaw pomocniczy znacznik czasu (µs)
  - `E_flow_Q_spin_time_T(&time)` — sprawdź, czy timer wygasł
  - `E_mouse_Q_pending_wheel` — pomocnicza zmienna do przechowywania delty wheel
- Warunki: `move>dx/dy`, `border` (x==0 || x==width-1), `timer expired` (`E_flow_Q_spin_time_T(&time)` == true)

---

## 1) Maszyna stanów przycisku (ASCII)

S_clean
  --(press)----------------------------------------------------> S_press
    [save coord; E_flow_Q_spin_time_M(&time, press_timeout); Yi_F(mouse, click, press_timeout)]

S_press
  --(release before press_timeout)-----------------------------> S_release
    [++click_count; E_flow_Q_spin_time_M(&time, release_timeout); Yi_F(mouse, click, release_timeout)]
  --(move > dx/dy) or (release with other conditions)---------> S_dirty
    [E_flow_Q_spin_time_M(&time, dirty_timeout); Yi_L(mouse, click)]
  --(press_timeout expires -> handler)-------------------------> S_press_held
    [handler sets press_held state]

S_release
  --(handler fired & same button)-----------------------------> S_press (increment click_count)
  --(handler fired & different or other)-----------------------> S_dirty

S_press_held
  --(release)-------------------------------------------------> S_press_held_release
    [E_flow_Q_spin_time_M(&time, 0); Yi_F(mouse, click, 0)]

S_dirty
  --(dirty_timeout expires)-----------------------------------> S_clean

NOTE: Ruch (move > dx/dy) powoduje wejście w `S_dirty` tylko jeśli bieżący stan to `S_press` lub `S_release`. Stan `S_press_held` blokuje wejście do `S_dirty`.

---

## 2) Border (krawędź) — uproszczony przebieg

Kiedy kursor dotrze do brzegu i spełnione warunki:
- `E_mouse_Q_button_S_click_count = ~0`
- `E_flow_Q_spin_time_M(&E_mouse_Q_button_S_time, border_timeout * 1000)`
- `Yi_F(mouse, click, border_timeout)`

Po upływie `border_timeout` handler może wykonać akcję brzegową (np. snap). Jeśli podczas odliczania nastąpi kliknięcie, klik ma priorytet i nadpisuje/anuluje odliczanie brzegowe.

---

## 3) Handler lifecycle + integracja wheel (ASCII)

Interrupt (case 3) dekoduje pakiet; `v` zawiera ostatni bajt (może zawierać rolkę):
  if wheel = (S8)v != 0:
    E_mouse_Q_pending_wheel = wheel
    E_flow_Q_spin_time_M(&E_mouse_Q_button_S_time, 0)    ; // pomocniczy znacznik
    Yi_F(mouse, click, 0)                               ; // użyj tego samego handlera

Handler (D(mouse, click)) — w krytycznej sekcji (IRQ zablokowane):
  if E_flow_Q_spin_time_T(&E_mouse_Q_button_S_time) && E_mouse_Q_button_S_state != S_dirty:
    if E_mouse_Q_pending_wheel != 0:
      wheel = E_mouse_Q_pending_wheel; E_mouse_Q_pending_wheel = 0;
      E_gui_Q_pointer_I_wheel(wheel)    ; // dispatch wheel — *nie zmieniaj stanów kliknięcia*
    else:
      E_gui_Q_pointer_I_click();       ; // normalny klik — wejścia do S_press_held / S_dirty zgodnie z logiką
  else:
    ; // handler rezygnuje (stan zmieniony / timer niespełniony)

Anulowanie:
- Kody, które przechodzą w `S_dirty` lub wywołują `Yi_L(mouse, click)`, powinny także czyścić `E_mouse_Q_pending_wheel = 0`, żeby zabronić późniejszego wykonania wheel po anulowaniu.

Priorytety:
- Kliknięcie/ruch prowadzący do `S_dirty` ma priorytet nad wheel (klik resetuje/anisuje schedulowany wheel).
- Wheel nie powinien zmieniać `E_mouse_Q_button_S_click_count` ani ustawiać `S_dirty`.

---

## 4) Testy proponowane
1. Wheel idle: symuluj wheel; oczekuj, że handler wywoła `E_gui_Q_pointer_I_wheel(delta)` i nie zmieni stanów kliknięcia.
2. Wheel + klik: scheduluj wheel; w krótkim czasie wywołaj klik (zmiana stanu) → klik powinien anulować wheel (GUI nie otrzymuje wheel).
3. Ruch > dx/dy podczas press: sprawdź, że `S_dirty` jest ustawiane i że schedulowany wheel zostaje odrzucony/wyczyszczony.
4. Border + wheel: wheel schedulowany podczas border odliczania → jeśli klik nastąpi przed expiry, klik ma priorytet (border/wheel nie wykonają akcji).

---

## 5) Referencje kodu
- Główne miejsce: `mouse.cx` — funkcja `E_mouse_I_interrupt` i handler `D(mouse, click)`.
- GUI: `gui.cx` — `E_gui_Q_pointer_I_click`, `E_gui_Q_pointer_I_wheel` (stub/placeholder).

---

Plik przygotowany jako baza do dalszej edycji; jeśli chcesz, mogę też wygenerować diagram PlantUML na podstawie tych ASCII-diagramów lub dodać grafikę SVG/PNG.
