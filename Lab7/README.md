### Обробка винятків і модульне тестування



Модернізація статичної бібліотеки з лабораторної роботи №4 (операції над поліномами). 

Додано клас Polynomial з контролем інваріантів, ієрархію власних винятків, 

гарантії безпеки щодо винятків, демонстрацію RAII і розкручування стека та модульні тести Google Test.

## 

Потрібні:  C++ і CMake, vcpkg, Git





##### Збирання і запуск

1\. Відкрити папку у Visual Studio, вибрати конфігурацію x64 Debug (vcpkg)

&#x20;  (vcpkg сам завантажить Google Test).

2\. Build -> Build All, запустити  PolyApp, AntipatternDemo, PolyTests

Preset використовує змінну VCPKG\_ROOT; якщо її немає, потрібно вказати шлях до

vcpkg.cmake у CMakePresets.json.



##### Інваріанти класу Polynomial



Для кожного об'єкта завжди виконується:



1\. коефіцієнтів хоча б один;

2\. коефіцієнтів не більше `kMaxCoefficients` (1000);

3\. кожен коефіцієнт скінченний (не NaN і не Inf).



Вектор коефіцієнтів закритий private`. Створити об'єкт можна лише через

конструктор, змінити коефіцієнт лише через `setCoefficient`, і обидва методи

перевіряють дані до запису. Конструктора без параметрів немає. 



##### Ієрархія винятків



std::runtime\_error

&#x20;|-- PolynomialException

&#x20;    |-- InvalidCoefficientException   (index, value)

&#x20;    |--PolynomialOverflowException   (operation)

&#x20;    |-- InvalidParameterException     (parameter, value)



Умова помилки	                                       Тип винятку

порожній набір коефіцієнтів	                        std::invalid\_argument

коефіцієнт NaN або Inf    	                        InvalidCoefficientException

індекс поза межами 	                                std::out\_of\_range

результат нескінченний                                PolynomialOverflowException

x або константа не скінченні	                InvalidParameterException

більше kMaxCoefficients коефіцієнтів 	std::length\_error





##### Гарантії безпеки

\- setCoefficient, multiplyAssign (\*=) — strong

\- swap, size, degree, coefficients — no-throw (noexcept)

\- решта методів об'єкт не змінюють; конструктор при винятку об'єкт не створює.



multiplyAssign спочатку обчислює добуток в окремому об'єкті, і лише після

успіху замінює коефіцієнти через swap, тому після винятку поліном незмінний.





##### RAII

Об'єкти ScopeLogger створюються в level1 -> level2 -> level3. У level3

конструктор Polynomial отримує NaN і кидає виняток, level2 ловить його і

передає далі через throw;





##### Антипатерни

catch (const std::exception\&) не ловить винятки int і std::string;

catch (...) ловить усе, але без типу й даних





##### Тести

21 тест у 7 групах: основні операції, точність (EXPECT\_NEAR), сценарії помилок

(EXPECT\_THROW), ієрархія, граничні значення, інваріанти, сильна гарантія.

Усі пройдено.

