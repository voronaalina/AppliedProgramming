\# Interpolation SDK, версія API 1.0



Бібліотека інтерполяції (лінійна та Лагранжа). Мова: C++ (C-ABI, extern "C").

Платформа: Windows x64, MSVC.



\## Склад пакунка

\- include/Interpolation.h: заголовковий файл (для неявного зв'язування)

\- lib/Interpolation.lib: імпортна бібліотека (для неявного зв'язування)

\- bin/Interpolation.dll: динамічна бібліотека (для обох способів)



\## Загальні правила

\- Усі функції повертають код помилки типу int (0 = успіх).

\- Результат записується у змінну за вказівником `result`.

\- Бібліотека не виділяє і не звільняє пам'ять. Усі масиви належать викликачу.

\- Імена експортів без декорування (extern "C"), підходять для GetProcAddress.

\- Усі числа типу double. Вхідні дані мають бути скінченними (не NaN, не inf).

\- Масиви x та y мають однакову довжину n. Обмеження: 2 <= n <= 10000.



\## Коди помилок

| Код | Назва | Значення |

|---|---|---|

| 0 | INTERP\_OK | успіх |

| -1 | INTERP\_ERR\_NULL | x, y або result дорівнює nullptr |

| -2 | INTERP\_ERR\_SIZE | n < 2 або n > 10000 |

| -3 | INTERP\_ERR\_DUPLICATE\_X | є однакові значення в x |

| -4 | INTERP\_ERR\_NOT\_SORTED | (linear) x не зростає |

| -5 | INTERP\_ERR\_OUT\_OF\_RANGE | (linear) xq поза \[x\[0], x\[n-1]] |

| -6 | INTERP\_ERR\_NOT\_FINITE | NaN або inf у вхідних даних |

| -7 | INTERP\_ERR\_BAD\_METHOD | невідомий method |

| -8 | INTERP\_ERR\_OVERFLOW | результат не скінченний |



\## Функції



\### int linearInterpolation(const double\* x, const double\* y, int n, double xq, double\* result)

\- Призначення: лінійна інтерполяція в точці xq.

\- x: вузли, строго зростають.

\- y: значення у вузлах.

\- xq: точка обчислення, x\[0] <= xq <= x\[n-1] (екстраполяції немає).

\- result: куди записується значення.

\- Помилки: -1, -2, -3, -4, -5, -6, -8.



\### int lagrange(const double\* x, const double\* y, int n, double xq, double\* result)

\- Призначення: значення інтерполяційного полінома Лагранжа в точці xq.

\- x: вузли, без дублікатів, порядок довільний.

\- y: значення у вузлах.

\- xq: будь-яке скінченне число (екстраполяція дозволена, але похибка може бути великою).

\- result: куди записується значення.

\- Помилки: -1, -2, -3, -6, -8.



\### int interpolate(const double\* x, const double\* y, int n, double xq, int method, double\* result)

\- Призначення: універсальний виклик.

\- method: 0 = лінійна, 1 = Лагранж.

\- Решта параметрів і обмеження як у обраного методу.

\- Помилки: усі вище та -7.



\### int interpolationError(const double\* x, const double\* y, int n, double xq, int method, double exactValue, double\* result)

\- Призначення: абсолютна похибка |exactValue - P(xq)| для обраного методу.

\- exactValue: точне значення функції в xq (скінченне).

\- Помилки: усі вище.



\## Приклад (неявне зв'язування)

&#x20;   double x\[] = {0, 1, 2, 3}, y\[] = {0, 1, 4, 9}, r;

&#x20;   int rc = lagrange(x, y, 4, 1.5, \&r);   // rc == 0, r == 2.25



\## Приклад (явне зв'язування)

&#x20;   typedef int (\*LagrangeFn)(const double\*, const double\*, int, double, double\*);

&#x20;   HMODULE h = LoadLibrary(L"Interpolation.dll");

&#x20;   LagrangeFn f = (LagrangeFn)GetProcAddress(h, "lagrange");



\## Версія

API 1.0. Збірка: Debug x64.

