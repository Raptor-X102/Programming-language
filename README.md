# Мой язык программирования

## Краткое описание
Это мой собственный язык программирования, в котором для синтаксиса используются различные эмодзи из символов.

## Пример программы

```
< main()

	n = 10
	a = fact(n)
	out(a)
	*(^_^)* 0
>

< fact(n)

	(o_0) n = 1
	(^o^):
		*(^_^)* n

	(=_=):
		*(^_^)* n * fact(n-1)
>
```

## Краткое описание бэкенда

Этот проект писался после [Дифференциатора](https://github.com/Raptor-X102/Differentiator), поэтому принцип схож.
Чтение и связывание лексем подробнее там.
Трансляция кода в ассемблерный происходит рекурсивно по дереву. Ассемблерный синтаксис компилируется в бинарный файл и исполняется на моем процессоре. Подробнее об этих проектах [здесь](https://github.com/Raptor-X102/Processor).

## Функционал

Объявление функций до и после main, рекурсивные функции (пример программы, высчитывающей факториал рекурсивно выше). Ветвления, цикл while.

## Синтаксис

Мой синтаксис смешал в себе Python (тело кода должно быть с отступом в Tab, без фигурных скобок) и мои другие задумки.

| Синтаксис        	| C эквивалент |
|-------------------|--------------|
| (o_0) expression 	| if           |
| (^o^): code      	| if true      |
| (=_=): code      	| else         |
| (-_-;)           	| while        |
| *(^_^)*          	| return       |
