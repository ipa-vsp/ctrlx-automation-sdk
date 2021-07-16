﻿/*
MIT License

Copyright (c) 2021 Bosch Rexroth AG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

using Datalayer;
using System;

namespace Samples.Datalayer.MQTT.Base
{
    /// <summary>
    /// Represents a provider node with an 1:1 relation to an address.
    /// </summary>
    internal class ProviderNodeWrapper
    {
        public ProviderNodeWrapper(string address, string name, IProviderNode node, IVariant value, bool isReadOnly)
        {
            Address = address;
            Name = name;
            Node = node;
            Value = value;
            IsReadOnly = isReadOnly;
        }

        public string Address { get; }
        public string Name { get; }
        public IProviderNode Node { get; }
        public IVariant Value { get; set; }
        public bool IsReadOnly { get; }
    }

    /// <summary>
    /// Provides extensions methods for IProvider
    /// </summary>
    internal static class ProviderExtensions
    {
        /// <summary>
        /// Creates and registers a node on a given address with a handler
        /// </summary>
        /// <param name="provider">The provider instance</param>
        /// <param name="baseAddress">The base node address</param>
        /// <param name="name">The name of the node appended to the base address</param>
        /// <param name="value">The value of type Variant</param>
        /// <param name="handler">The handler instance</param>
        /// <param name="isReadOnly">Indicates whether the node is read-only</param>
        /// <returns>The created node wrapper.</returns>
        public static (DLR_RESULT, ProviderNodeWrapper) CreateNode(this IProvider provider, IProviderNodeHandler handler, string baseAddress, string name, IVariant value, bool isReadOnly = false)
        {
            //Register the node
            var address = $"{baseAddress}/{name}";
            var (result, node) = provider.RegisterNode(address, handler);
            if (result.IsBad())
            {
                return (result, null);
            }

            Console.WriteLine($"Created a node on registered address: {address}");
            return (result, new ProviderNodeWrapper(address, name, node, value, isReadOnly));
        }
    }
}