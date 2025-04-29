using System;
using System.Security.Cryptography;
using System.Text;

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
            string[] conf_arr = {"t9jQo4UCbK2ZCYwUUSBf2oYT7q1ogMGVrgjUqWnzqLxMXw3GIeVZpids5gIz2YZu", "3qBjH4yDUHjhZBxWK56eYw==" , "P/4B29PWaJ6Raw+51xox2A==" ,
                                 "fwWlqX1XMU7EFmHRUHk3Jw==" , "TowG+c1OR3RBmATvJwUFKQ==" , "lXEVYeoDw31nYYF2ts9aUQ==" , "gcbmRCfQRwasaegNU1/NvQ==" ,
                                 "sJHKF5x7kjxy85oLMym05A==" , "llBblX1iqHd1zfZIV8Z0jL3MzbCo6zP7QWx7R9nEvuQbIA25kxWNjjY8WYEY+Xh1" ,
                                 "ILq1reLnyJdhfez8kYLyBYJr+EjguBMQ6n4dPjgAia6wJGxs5SWbzuMPh1LUk/Ig" , "6I60HSsPViAp3nyv1OYEEQ=="
            };
            int i = 0;
            foreach (string conf in conf_arr)
            {
                i++;
                string x = Convert.ToString(decrypt(conf));
                Console.WriteLine("The decrypteed value number " + i + " is: " + x);
            }
        }
    }
}
