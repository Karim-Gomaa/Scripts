using System;
using System.Security.Cryptography;
using System.Text;
using System.Collections.Generic;
using System.Reflection;

namespace Xworm
{
    internal class Program
    {
        public static byte[] get_byts_encoded(string Tzme3PWI6TL0OkpZdAFJ)
        {
            return Encoding.UTF8.GetBytes(Tzme3PWI6TL0OkpZdAFJ);
        }

        public static string get_string_ecoding(byte[] DVqcQRyUiEoVe72AbaTd)
        {
            return Encoding.UTF8.GetString(DVqcQRyUiEoVe72AbaTd);
        }

        public static object decrypt(string encrypted)
        {
            RijndaelManaged rijndaelManaged = new RijndaelManaged();
            MD5CryptoServiceProvider md5CryptoServiceProvider = new MD5CryptoServiceProvider();
            byte[] array = new byte[32];
            byte[] sourceArray = md5CryptoServiceProvider.ComputeHash(get_byts_encoded("8xTJ0EKPuiQsJVaT"));
            Array.Copy(sourceArray, 0, array, 0, 16); // move key hash to first 16 bytes
            Array.Copy(sourceArray, 0, array, 15, 16); // move key hash to last 16 bytes
            rijndaelManaged.Key = array;
            rijndaelManaged.Mode = CipherMode.ECB;
            ICryptoTransform cryptoTransform = rijndaelManaged.CreateDecryptor();
            byte[] array2 = Convert.FromBase64String(encrypted);
            return get_string_ecoding(cryptoTransform.TransformFinalBlock(array2, 0, array2.Length));
        }

        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("Usage: Xworm_Config_Extractor.exe <File_Full_Path>");
                return;
            }
            //string filePath = "D:\\Malware\\EG-CERT\\s1\\Sample1";
            string filePath = args[0];
            List<string> confArr = new List<string>();

            // Array of field names to extract
            string[] names = {
                "ZIDZvDLAFbRYxsxkwMl1lB7DELyeP0rfiJNEILKuap1H9eXgbiPbiwGYX2g2",
                "PjOzPaAZem6YRSiY73iqOnuhSIsTpJmmeYR23TelLywq50KJA7ITRso6eQWj",
                "uOJTJdC1HuPnY7xGUKCVXdob11jaXDot6DaLkEHtlik255I34dAgKgpePnrM",
                "tEcXo2432ATvkU8ifrMlTSOiGO1G3sGNZRTy6G0EbDFiN3BWkUbrhUYMxUZC",
                "QMNuWZu2nAhrN4vCJXXVJJ6rXPYjheog2O3JKqbePCETj5t8Y1KaOTCSa7k0",
                "s6qNUlBh1I6DXfxJKXLS8vMqDb2zNIYNi5hhilJnX0Mbzr8B4g6F0vguJvMV",
                "pAiK6SOy8HEO6uDLFXMlZSPdAbNKgcHqwR32QBERmGnbcKxg5SelHoKfUgGc",
                "EB5J4sIzfH74BwfgRjacCtnEuNWFxu93z57nr4HrttTW5asXOhadv7pC7YFu",
                "WZkYJQQccjD5T1CeURgUhXfKErUOd2iOmZLqE3X2ot4M56ME6ZG8zQR2Ub1G",
                "TWpgNdrzPbXNxjOwjHl7Bk3kQzFwaIkHgioRO2b6uJ9qXBYpgIkrYVEP0YDx",
                "39U6klKkfRunu4AJWlFFg8Gj1E3vVNiGNrr5yGLn1VkgOUqSSb0FojA3RYMT"
            };

            try
            {
                // Load the assembly from file
                Assembly assembly = Assembly.LoadFile(filePath);

                // Specify the target class name
                string className = "NB2mi1VBTSN5U40DfEsDcrzgxWCrxt7i1yCoMW0Zb5dK9QwIjZ6W6wYeHriq";
                Type classType = assembly.GetType(className);

                if (classType == null)
                {
                    Console.WriteLine("Class not found in assembly.");
                    return;
                }

                // Create an instance of the class
                object classInstance = Activator.CreateInstance(classType);

                // Extract specified fields
                bool hasFields = false;
                foreach (var field in classType.GetFields())
                {
                    if (Array.Exists(names, name => name == field.Name))
                    {
                        hasFields = true;
                        var fieldValue = field.GetValue(classInstance);
                        confArr.Add(fieldValue?.ToString() ?? "null");
                    }
                }

                // Output the configuration
                if (!hasFields)
                {
                    Console.WriteLine("Cannot extract the configuration.");
                }
                else
                {
                    Console.WriteLine("Sample configuration:");
                    foreach (var value in confArr)
                    {
                        Console.WriteLine(value);
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error: {ex.Message}");
            }
            // Read each encrypted string from command-line arguments
            int i = 0;
            foreach (string conf in confArr)
            {
                i++;
                try
                {
                    string decryptedValue = Convert.ToString(decrypt(conf));
                    Console.WriteLine("The decrypted value number " + i + " is: " + decryptedValue);
                }
                catch (Exception e)
                {
                    Console.WriteLine("Decryption failed for value number " + i + ": " + e.Message);
                }
            }
        }
    }
}
