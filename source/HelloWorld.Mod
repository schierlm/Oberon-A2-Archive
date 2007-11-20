MODULE HelloWorld;

IMPORT
	AosOut;

PROCEDURE Hello*;
BEGIN
	AosOut.String("Hello"); AosOut.Ln; 
END Hello;

PROCEDURE HelloCn*;
BEGIN
	AosOut.String("你好"); AosOut.Ln;
END HelloCn;

PROCEDURE HelloKr*;
BEGIN
	AosOut.String("여보세요"); AosOut.Ln; 
END HelloKr;

PROCEDURE HelloJp*;
BEGIN
	AosOut.String("こんにちは"); AosOut.Ln; 
END HelloJp;

PROCEDURE HelloRu*;
BEGIN
	AosOut.String("привет"); AosOut.Ln; 
END HelloRu;

END HelloWorld.

S.Free HelloWorld ~
HelloWorld.Hello ~
HelloWorld.HelloCn ~
HelloWorld.HelloKr ~
HelloWorld.HelloJp ~ 
HelloWorld.HelloRu ~
 

