# ارسال پیامک سریع

&#x202b;
این مخزن یک پروژه بسیار کوچیک , برای ارسال سریع پیامک از طریق sms.ir میباشد.

# راهنمای استفاده

&#x202b;
لطفا دستور های زیر را به ترتیب وارد کنید.

```cmake -B build -H.  ```  
```cd build  ```  
```make  ```  
```./sms_ir  (run program)```

## الزامات
```cmake minimum version => 3.16  ```  
```hiredis  ```  
```redis++  ```  
```curl  ```  
```curlpp  ```  
```json(nlohmann)   ```

## مثال
&#x202b;
برای ارسال پیامک , شما باید آرایه ای به شکل زیر به کانالی با نام sendSms در redis ارسال کنید تا پیامک شما ارسال شود.

```
[
    'type' => 'advanced' ,
    'templateId' => templateId ,
    'parameters' => [
        ['key' => 'key' , 'value' => 'value']
    ] ,
    'phone' => 09********* 
]
```
