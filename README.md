# Курсовая работа
Небольшая программа, визуализирующая множество точек на поверхности сферы с помощью **OpenGL**. Программа позволяет применять к точкам сферы повороты (вокруг осей, проходящих через центр сферы) для изучения трансформации точек. Полученное изображение можно вращать и приближать/отдалять с помощью мыши или клавиатуры. Частичный обзор кода и пример работы программы можно найти в [тексте курсовой работы](/Текст%20курсовой.docx).

Используемые сторонние библиотеки: [GLFW](https://github.com/glfw/glfw), [glad](https://github.com/Dav1dde/glad), [Dear ImGui](https://github.com/ocornut/imgui), [GLM](https://github.com/g-truc/glm).

<sub>P.S. Почему сама программа, комментарии в коде и этот README на русском, но комментарии к коммитам на английском? Потому что я начал писать их на английском по привычке и решил не менять язык в середине разработки.</sub>

## Указания по клонированию:
Так как **GLFW** и **GLM** подключены как подмодули, клонировать репозиторий нужно с флагом `--recurse-submodules`:
```
git clone --recurse-submodules https://github.com/qqpilla/coursework-2
```
В случае, если репозиторий был клонирован без указания флага `--recurse-submodules`, можно воспользоваться следующими командами:
```
git submodule init
git submodule update
```
или коротко:
```
git submodule update --init
```
## Указания по сборке
На вашей системе должен быть установлен [**CMake**](https://cmake.org/download/) (исходный код и указания по сборке можно найти [здесь](https://github.com/Kitware/CMake)).

Создайте директорию `build` и перейдите в неё (`cd build`). Воспользуйтесь следующей командой для генерации файлов **CMake**:
```
cmake -G <generator-name> <path-to-source>
```
, где вам нужно указать имя генератора, который вы хотите использовать, и путь к корневой папке проекта. Например, при вызове команды из директории `build`, расположенной по пути `.../coursework-2/build`, для сборки проекта с помощью **Visual Studio 2022**, синтаксис будет таким:
```
cmake -G "Visual Studio 17 2022" ..
```
Опцию `-G <generator-name>` можно опустить. Тогда **CMake** сам выберет компилятор в зависимости от вашей системы. Список всех компиляторов, доступных на вашей платформе, вы можете увидеть с помощью команды `cmake --help`.

Наконец, чтобы собрать проект, вызовите команду ``cmake --build .`` из директории `build` (или сделайте это с помощью любых других удобных вам инструментов, таких как **Visual Studio** или **Make**). Вы найдёте исполняемый файл с названием `program` внутри директории `build` или одной из её поддиректорий (в зависимости от выбранного генератора).

## Управление
- Зажмите **ПКМ** или используйте стрелки на клавиатуре, чтобы вращать изображение.
- Используйте **колесо мыши** или стрелки **вверх**/**вниз** с зажатой клавишей **Shift**, чтобы приблизить/отдалить изображение.
- Нажмите клавишу **Escape**, чтобы закрыть приложение.

С помощью *окна свойств* вы можете изменить уровень детализации сферы (количество точек на ней), её цвет и включить/выключить её отображение. 

Каждый поворот, применяемый к сфере, порождает новое множество точек (т.е. новую сферу), и с помощью всё того же окна вы можете изменять свойства поворотов: задавать угол наклона, ось вращения, цвет получаемой в результате поворота сферы, а также включать/выключать отображение этой сферы (изначально все такие сферы выключены). У первых поворотов также есть *дети* - повороты, которые применяются к точкам не изначальной сферы, а сферы, порождаемой *поворотом-родителем*. Свойства *поворотов-детей* также можно изменять. Чтобы увидеть эти повороты в окне свойств, нажмите на маленькую стрелочку слева от поворота-родителя.

Второе окно показывает точки, принадлежащие изначальной сфере, и результаты применения поворотов к точкам этой сферы (а также результаты применения поворотов-детей к точкам сфер, порождаемых поворотами-родителями). Результаты применения поворота к точке выводятся в виде: `(A.x, A.y, A.z) ---> (B.x, B.y, B.z)`, где `A` - координаты точки до преобразования, а `B` - координаты точки после. Опция `"Использовать стилизованный текст"` окрашивает записи о координатах точек в цвета сфер, которым эти точки принадлежат. Например, если цвет изначальной сферы *красный*, а цвет сферы, полученной при повороте, *синий*, то текст `(A.x, A.y, A.z)` будет красного цвета, а `(B.x, B.y, B.z)` - синего.

Вы также можете ввести точки изначальной сферы вручную. Для этого создайте файл с названием `input.txt` в корневой папке проекта, в который введите сначала число точек, а затем координаты точек через пробел, т.е. так:
```
n
1.x 1.y 1.z
2.x 2.y 2.z
3.x 3.y 3.z
...
n.x n.y n.z
```
Если существует файл `input.txt`, возможности изменить уровень детализации сферы нет (даже если этот файл пустой!)